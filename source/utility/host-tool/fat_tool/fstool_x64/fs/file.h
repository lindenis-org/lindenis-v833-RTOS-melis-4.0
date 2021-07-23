/*
 * Wrapper functions for accessing the file_struct fd array.
 */

#ifndef __LINUX_FILE_H
#define __LINUX_FILE_H

struct file;
extern void fput(struct file *);

static __inline void fput_light(struct file *file, int fput_needed)
{
//  if (fput_needed)
//      fput(file);
}

extern void fget(struct file *);
extern void put_filp(struct file *);

#endif /* __LINUX_FILE_H */
