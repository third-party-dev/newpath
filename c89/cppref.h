/* Original Code From cppreference.com 
     - https://en.cppreference.com/w/c/string/multibyte/mblen
   Licenses:
     - https://en.cppreference.com/w/Cppreference:Copyright/GDFL
     - https://en.cppreference.com/w/Cppreference:Copyright/CC-BY-SA
     - http://creativecommons.org/licenses/by-sa/3.0/
*/

/* (Private) Counts the total multi-byte characters in memory. */
/* Note: This includes non-visible variation selectors. */
static size_t strlen_mb(
    const char* path,
    const size_t path_len)
{
    size_t result = 0;
    char *ptr = (char *)path;
    const char* end = ptr + path_len;
    mblen(NULL, 0); /* reset the conversion state */
    while(ptr < end) {
        int next = mblen(ptr, end - ptr);
        if(next == -1) {
           return (size_t)-1;
           break;
        }
        ptr += next;
        ++result;
    }
    return result;
}