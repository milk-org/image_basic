/** @file streamfeed.h
 */


errno_t __attribute__ ((cold)) streamfeed_addCLIcmd();

long IMAGE_BASIC_streamfeed(
    const char *__restrict__ IDname,
    const char *__restrict__ streamname,
    float       frequ
);
