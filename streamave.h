/** @file streamave.h
 */

errno_t __attribute__((cold)) streamave_addCLIcmd();

imageID IMAGE_BASIC_streamaverage(const char *__restrict IDname,
                                  long NBcoadd,
                                  const char *__restrict IDoutname,
                                  int mode,
                                  int semindex);
