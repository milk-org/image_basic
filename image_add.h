/** @file image_add.h
 */

errno_t image_add_addCLIcmd();


imageID basic_add(
    const char *__restrict__ ID_name1,
    const char *__restrict__ ID_name2,
    const char *__restrict__ ID_name_out,
    long off1,
    long off2
);


imageID basic_add3D(
    const char *__restrict__ ID_name1,
    const char *__restrict__ ID_name2,
    const char *__restrict__ ID_name_out,
    long        off1,
    long        off2,
    long        off3
);

