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
    int32_t count = 0;
    for (int i = 0; str[i] != '\0'; i++) 
    {
        if (islower(str[i])) {
            str[i] = toupper(str[i]);
            count++;
        }
    }
    return count;
}

int32_t width_from_start_byte(char start_byte) 
{
    unsigned char byte = (unsigned char)start_byte;
    if ((byte >> 7) == 0) 
    {
        return 1;
    } else if ((byte >> 5) == 0b110) 
    {
        return 2;
    } else if ((byte >> 4) == 0b1110) 
    {
        return 3;
    } else if ((byte >> 3) == 0b11110) 
    {
        return 4;
    } else {
        return -1;
    }
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
    int32_t byte_idx = codepoint_index_to_byte_index(str, cpi);
    if (byte_idx == -1) 
    {
        return -1;
    }
    unsigned char byte = (unsigned char)str[byte_idx];
    int32_t codepoint = -1;
    if ((byte >> 7) == 0) 
    {
        codepoint = byte;
    } else if ((byte >> 5) == 0b110) 
    {
        codepoint = ((byte & 0b00011111) << 6) | ((unsigned char)str[byte_idx + 1] & 0b00111111);
    } else if ((byte >> 4) == 0b1110) 
    {
        codepoint = ((byte & 0b00001111) << 12) | (((unsigned char)str[byte_idx + 1] & 0b00111111) << 6) | ((unsigned char)str[byte_idx + 2] & 0b00111111);
    } else if ((byte >> 3) == 0b11110) 
    {
        codepoint = ((byte & 0b00000111) << 18) | (((unsigned char)str[byte_idx + 1] & 0b00111111) << 12) | (((unsigned char)str[byte_idx + 2] & 0b00111111) << 6) | ((unsigned char)str[byte_idx + 3] & 0b00111111);
    } else {
        return -1;
    }
    return codepoint;
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

    return 0;
}
