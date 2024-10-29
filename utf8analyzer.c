#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

int32_t is_ascii(char str[]) 
{
    for (int i = 0; str[i] != '\0'; i++) 
    {
        if ((unsigned char)str[i] > 127) 
        {
            return 0;
        }
    }
    return 1;
}

int32_t capitalize_ascii(char str[]) 
{
    uint32_t index = 0;
    int32_t count = 0;
    while (str[index] != 0) 
    {
        if (str[index] >= 'a' && str[index] <= 'z') 
        {
            str[index] -= 32;
            count++;
        }
        index += 1;
    }
    return count;
}

int32_t width_from_start_byte(char start_byte) 
{
    unsigned char byte = (unsigned char)start_byte;
    if (byte < 0x80) 
    {
        return 1;
    }
    else if (byte >= 0xC0 && byte <= 0xDF) 
    {
        return 2;
    }
    else if (byte >= 0xE0 && byte <= 0xEF) 
    {
        return 3;
    }
    else if (byte >= 0xF0 && byte <= 0xF7) 
    {
        return 4;
    }
    return -1;
}

int32_t utf8_strlen(char str[]) 
{
    int32_t length = 0;
    for (int i = 0; str[i] != '\0';) 
    {
        int32_t width = width_from_start_byte(str[i]);
        if (width == -1) 
        {
            return -1; 
        }
        length++;
        i += width;
    }
    return length;
}

int32_t codepoint_index_to_byte_index(char str[], int32_t cpi) 
{
    int32_t byte_index = 0;
    int32_t codepoint_count = 0;
    for (int i = 0; str[i] != '\0';) 
    {
        if (codepoint_count == cpi) 
        {
            return byte_index;
        }
        int32_t width = width_from_start_byte(str[i]);
        if (width == -1) 
        {
            return -1; 
        }
        byte_index += width;
        codepoint_count++;
        i += width;
    }
    return (codepoint_count == cpi) ? byte_index : -1;
}

void utf8_substring(char str[], int32_t cpi_start, int32_t cpi_end, char result[]) 
{
    int32_t length = utf8_strlen(str);
    if (cpi_end > length) 
    {
        cpi_end = length;
    }
    int32_t byte_start = codepoint_index_to_byte_index(str, cpi_start);
    int32_t byte_end = codepoint_index_to_byte_index(str, cpi_end);
    if (byte_start == -1 || byte_end == -1 || cpi_start > cpi_end) 
    {
        result[0] = '\0';
        return;
    }
    strncpy(result, str + byte_start, byte_end - byte_start);
    result[byte_end - byte_start] = '\0';
}

int32_t codepoint_at(char str[], int32_t cpi) 
{
    int byte_index = codepoint_index_to_byte_index(str, cpi);
    char *codepoint = (str + byte_index); 

    int width = width_from_start_byte(codepoint[0]);

    if (width == 1) 
    {
        return codepoint[0];
    }

    if (width == 2) 
    {
        return ((codepoint[0] & 0x1F) << 6) + (codepoint[1] & 0x3F);
    }
    if (width == 3) 
    {
        return ((codepoint[0] & 0x0F) << 12) + ((codepoint[1] & 0x3F) << 6) + (codepoint[2] & 0x3F);
    }
    if (width == 4) 
    {
        return ((codepoint[0] & 0x07) << 18) + ((codepoint[1] & 0x3F) << 12) + ((codepoint[2] & 0x3F) << 6) + (codepoint[3] & 0x3F);
    }

    return -1;
}

char is_animal_emoji_at(char str[], int32_t cpi) 
{
    int32_t codepoint = codepoint_at(str, cpi);
    if ((codepoint >= 0x1F400 && codepoint <= 0x1F43F) || (codepoint >= 0x1F980 && codepoint <= 0x1F99E) || (codepoint >= 0x1F600 && codepoint <= 0x1F64F)) 
    { 
        return 1;
     }
    return 0;
}

// Resubmission requirement
void next_utf8_char(char str[], int32_t cpi, char result[]) 
{
    int32_t codepoint = codepoint_at(str, cpi);
    if (codepoint == -1) 
    {
        result[0] = '\0';
        return;
    }

    int32_t next_codepoint = codepoint + 1;
    if (next_codepoint <= 0x10FFFF) 
    {
        result[0] = 0xF0 | ((next_codepoint >> 18) & 0x07);
        result[1] = 0x80 | ((next_codepoint >> 12) & 0x3F);
        result[2] = 0x80 | ((next_codepoint >> 6) & 0x3F);
        result[3] = 0x80 | (next_codepoint & 0x3F);
        result[4] = '\0';
    } else 
    {
        result[0] = '\0';
    }
}



int main() 
{
    char str[256];
    printf("Enter a UTF-8 encoded string: ");
    fgets(str, 256, stdin);
    str[strcspn(str, "\n")] = 0; 

    char original_str[256];
    strcpy(original_str, str);
    printf("Valid ASCII: %s\n", is_ascii(str) ? "true" : "false");
    
    capitalize_ascii(str);
    printf("Uppercased ASCII: \"%s\"\n", str);
    
    printf("Length in bytes: %lu\n", strlen(str));
    printf("Number of code points: %d\n", utf8_strlen(str));
    
    printf("Bytes per code point: ");
    for (int i = 0; str[i] != '\0';) 
    {
        int32_t width = width_from_start_byte(str[i]);
        printf("%d ", width);
        i += width;
    }
    printf("\n");
    
    char result[256] = {0};
    utf8_substring(original_str, 0, 6, result);
    printf("Substring of the first 6 code points: \"%s\"\n", result);
    
    printf("Code points as decimal numbers: ");
    for (int i = 0; i < utf8_strlen(original_str); i++) 
    {
        printf("%d ", codepoint_at(original_str, i));
    }
    printf("\n");
    
    printf("Animal emojis: ");
    for (int i = 0; i < utf8_strlen(str); i++) 
    {
        if (is_animal_emoji_at(str, i)) 
        {
            int byte_index = codepoint_index_to_byte_index(str, i);
            int width = width_from_start_byte(str[byte_index]);
            for (int j = 0; j < width; j++) 
            {
                printf("%c", str[byte_index + j]);
            }
        }
    }
    printf("\n");


    int idx = 3;
    next_utf8_char(str, idx, result);
    printf("Next Character of Codepoint at Index 3: %s\n", result);

    return 0;
}
