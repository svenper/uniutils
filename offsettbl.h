#define OFT_UNDEF (-1)
#define OFT_NOINF (-2)

struct oft {
  UTF32 b;
  UTF32 e;
  long offset; /* Must be SIGNED */
};
