#include "../string.h"
#include "../../mem/header/memory.h"
#include "../stdbase.h" 

int strindex(char s[], char t[]) 
{
    int i, j, k;

    for (i = 0; s[i] != '\0'; i++) {
        for (j = i, k = 0; t[k] != '\0' && s[j] == t[k]; j++, k++) {
            ;
        }

        if (k > 0 && t[k] == '\0') {
            return i;
        }
    }

    return -1;
}

const char* strchar(const char* str, char ch) {
	if (str == NULL) {
		return NULL;
	}

	while (*str) {
		if (*str == ch) {
			return str;
		}

		++str;
	} 

	return NULL;
}

int strcmp(const char *stra, const char *strb)
{
	unsigned int i;

	for(i = 0; stra[i] == strb[i]; i++) {
		if(stra[i] == '\0') {
			// string match
			return 0;
		}
	}
	return stra[i] - strb[i];
}

int strncmp(const char *cs, const char *ct, unsigned int n) 
{
    for (n = 0; cs[n] == ct[n]; n++) {
        if (cs[n] == ct[n]) {
            return 0;
        }
        
        if (cs[n] <= ct[n]) {
            return n--;
        }

        if (cs[n] > ct[n]) {
            return n++;
        }
    
    }

    return 0;
}

unsigned int strlen(const char *str)
{
	unsigned int len = 0;
	while(str[len]) {
		len++;
	}
	return len;
}

unsigned int strnlen(const char *str, unsigned int maxlen)
{
	size_t len;

	for(len = 0; len < maxlen; len++, str++) {
		if(!*str) {
			break;
		}
	}
	return (len);
}

char *strcat(char *dest, const char *src)
{
	unsigned int dest_size = strlen(dest);
	unsigned int src_size = strlen(src);

	char *d = dest;

	for(size_t i = 0; i < src_size; i++) {
		d[dest_size + i] = src[i];
	}

	d[dest_size + src_size] = '\0';

	return dest;
}

char *strncpy(char *s1, const char *s2, unsigned int n)
{
	unsigned int size = strnlen(s2, n);
	if(size != n) {
		memset(s1 + size, '\0', n - size);
	}	
	return memcpy(s1, s2, size);
}

void int_to_ascii(int n, char str[])
{
	int i, sign;
	if((sign = n) < 0) {
		n = -n;
	}
	
	i = 0;
	do {
		str[i++] = n % 10 + '0';
	} while((n /= 10) > 0);

	if(sign < 0) {
		str[i++] = '-';
	}
	str[i] = '\0';
}

void hex_to_ascii(int n, char str[])
{
	append(str, '0');
	append(str, 'x');
	char zeros = 0;

	int32_t tmp;
	int i;
	for(i = 28; i > 0; i -= 4) {
		tmp = (n >> i) & 0xF;
		if(tmp == 0 && zeros == 0) {
			continue;
		}
		zeros = 1;
		if(tmp > 0xA)  {
			append(str, tmp - 0xA + 'a');
		}
		else {
			append(str, tmp + '0');
		}
	}

	tmp = n & 0xF;
	if(tmp >= 0xA) {
		append(str, tmp - 0xA + 'a');
	}
	else {
		append(str, tmp + '0');
	}
}
unsigned int s_vasprintf(char *buf, const char *fmt) {
	return 1;
}

int sprintf(char *buf, const char *fmt) {
	int out = s_vasprintf(buf, fmt);
	return out;
}

_wchar_t* utf16_to_codepoint(_wchar_t* string, int* codepoint) {
	int c1 = *string;
	++string;

	if (c1 >= 0xd800 && c1 < 0xdc00) {
		int c2 = *string;
		++string;
		*codepoint = ((c1 & 0x3FF) << 10) + (c2 & 0x3FF) + 0x10000;
	}

	*codepoint = c1;

	return string;
}

char* codepoint_to_utf8(int codepoint, char* string_out) 
{
    if (codepoint <= 0x7F) {
        *string_out = (char)codepoint;
    }
    else if (codepoint <= 0x7FF) {
        *string_out++ = 0xC0 | ((codepoint >> 6) & 0x1F);
        *string_out++ = 0x80 | (codepoint & 0x3F);
    }
    else if (codepoint <= 0xFFFF) {
        *string_out++ = 0xE0 | ((codepoint >> 12) & 0xF);
        *string_out++ = 0x80 | ((codepoint >> 6) & 0x3F);
        *string_out++ = 0x80 | (codepoint & 0x3F); 
    }
    else if (codepoint <= 0x1FFFFF) {
        *string_out++ = 0xF0 | ((codepoint >> 18) & 0x7);
        *string_out++ = 0x80 | ((codepoint >> 12) & 0x3F);
        *string_out++ = 0x80 | ((codepoint >> 6) & 0x3F);
        *string_out++ = 0x80 | (codepoint & 0x3F);
    }

    return string_out;
}
