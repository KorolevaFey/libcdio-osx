/* -*- C -*-
  Copyright (C) 2010 Rocky Bernstein <rocky@gnu.org>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* 
   Unit test for lib/driver/gnu_linux.c
*/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_STDIO_H
# include <stdio.h>
#endif
#ifdef HAVE_STRING_H
# include <string.h>
#endif
#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_ERRNO_H
# include <errno.h>
#endif

#include <cdio/util.h>

#define MY_DIR_SEPARATOR '/'

static char *
get_temporary_name(const char *dirname, const char *errmsg)
{
    char *new_filename = tempnam(NULL, "syml");
    if (NULL == new_filename) {
	printf("Could not generate %s name\n", errmsg);
    }
    return new_filename;
}

static int check_rc(int rc, const char *psz_operation,
    const char *psz_filename)
{
    if (-1 == rc) {
	printf("%s %s failed with error: %s\n", 
	       psz_operation, psz_filename, strerror(errno));
    } else if (0 != rc) {
	printf("%s %s gives weird return %d\n", 
	       psz_operation, psz_filename, rc);
    }
    return rc;
}



int
main(int argc, const char *argv[])
{
    char *psz_tmp_subdir;
    char *psz_orig_file;
    char tmp_dir[PATH_MAX+1];
    char tmp_subdir[PATH_MAX+1];
    char psz_file_check[PATH_MAX+1];
    char *psz_last_slash;
    unsigned int i_last_slash;
    char *psz_symlink_file = NULL;

    psz_tmp_subdir = get_temporary_name(NULL, "temporary directory");
    if (NULL == psz_tmp_subdir) {
	exit(77);
    }
    psz_last_slash = strrchr(psz_tmp_subdir, MY_DIR_SEPARATOR);
    if (NULL == psz_tmp_subdir) {
	printf("extract parent directory from temporary directory name\n");
	exit(77);
    }
    i_last_slash = psz_last_slash - psz_tmp_subdir + 1;
    memcpy(tmp_dir, psz_tmp_subdir, i_last_slash);
    tmp_dir[i_last_slash] = '\0';
    printf("Temp directory is %s\n", tmp_dir);

    if (-1 == check_rc(mkdir(psz_tmp_subdir, 0755),
		       "mkdir", psz_tmp_subdir))
	exit(77);
    
    psz_orig_file = get_temporary_name(NULL, "file");
    if (NULL != psz_orig_file) {
	FILE *fp = fopen(psz_orig_file, "w");
	int rc;
	fprintf(fp, "testing\n");
	fclose(fp);
	psz_symlink_file = get_temporary_name(NULL, "symlink file");
	rc = check_rc(symlink(psz_orig_file, psz_symlink_file),
		      "unlink", psz_symlink_file);
	if (0 == rc) {
	    /* Just when you thought we'd forgotten, here is our first
	       test! */
	    cdio_follow_symlink(psz_symlink_file, psz_file_check);
	    if (0 != strncmp(psz_file_check, psz_orig_file, PATH_MAX)) {
		fprintf(stderr, "simple cdio_follow_symlink failed. %s vs %s",
			psz_file_check, psz_orig_file);
		exit(1);
	    }
	    
		
	}
	    
    }
    
    if (NULL != psz_symlink_file)
	check_rc(unlink(psz_symlink_file), "unlink", psz_symlink_file);
    check_rc(unlink(psz_orig_file), "unlink", psz_orig_file);
    check_rc(rmdir(psz_tmp_subdir), "rmdir", psz_tmp_subdir);

    return 0;
}