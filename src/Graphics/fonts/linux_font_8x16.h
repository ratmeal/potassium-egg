#ifndef linux_font_8x8.h
#define linux_font_8x8.h
#define FONTDATAMAX 4096
#define FONT_EXTRA_WORDS 4
#define VGA8x16_IDX	1
struct font_data {
	unsigned int extra[FONT_EXTRA_WORDS];
	const unsigned char data[];
} __attribute__((packed));

struct font_desc {
    int idx;
    const char *name;
    unsigned int width, height;
    unsigned int charcount;
    const void *data;
    int pref;
};
extern const struct font_data fontdata_8x16;
#endif