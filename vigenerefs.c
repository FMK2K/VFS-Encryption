//
// Created by FeisalK on 5/2/2024.
// Refrenced https://www.geeksforgeeks.org/vigenere-cipher/
//

#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

static char* storage_dir        = NULL;
static char  storage_path[256];
//Keyword
static string   keyword = "AYUSH";
string key = "";
fprintf("Keyword: %s\n", keyword);

char* prepend_storage_dir (char* pre_path, const char* path) {
  strcpy(pre_path, storage_dir);
  strcat(pre_path, path);
  return pre_path;
}

static int vigenere_getattr(const char *path, struct stat *stbuf)
{
	int res;
	
	path = prepend_storage_dir(storage_path, path);
	res = lstat(path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int vigenere_access(const char *path, int mask)
{
	int res;

	path = prepend_storage_dir(storage_path, path);
	res = access(path, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int vigenere_readlink(const char *path, char *buf, size_t size)
{
	int res;

	path = prepend_storage_dir(storage_path, path);
	res = readlink(path, buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}


static int vigenere_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	path = prepend_storage_dir(storage_path, path);
	dp = opendir(path);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int vigenere_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	path = prepend_storage_dir(storage_path, path);
	if (S_ISREG(mode)) {
		res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(path, mode);
	else
		res = mknod(path, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int vigenere_mkdir(const char *path, mode_t mode)
{
	int res;

	path = prepend_storage_dir(storage_path, path);
	res = mkdir(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int vigenere_unlink(const char *path)
{
	int res;

	path = prepend_storage_dir(storage_path, path);
	res = unlink(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int vigenere_rmdir(const char *path)
{
	int res;

	path = prepend_storage_dir(storage_path, path);
	res = rmdir(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int vigenere_symlink(const char *from, const char *to)
{
	int res;
	char storage_from[256];
	char storage_to[256];

	prepend_storage_dir(storage_from, from);
	prepend_storage_dir(storage_to,   to  );
	res = symlink(storage_from, storage_to);
	if (res == -1)
		return -errno;

	return 0;
}

static int vigenere_rename(const char *from, const char *to)
{
	int res;
	char storage_from[256];
	char storage_to[256];

	prepend_storage_dir(storage_from, from);
	prepend_storage_dir(storage_to,   to  );
	res = rename(storage_from, storage_to);
	if (res == -1)
		return -errno;

	return 0;
}

static int vigenere_link(const char *from, const char *to)
{
	int res;
	char storage_from[256];
	char storage_to[256];

	prepend_storage_dir(storage_from, from);
	prepend_storage_dir(storage_to,   to  );
	res = link(storage_from, storage_to);
	if (res == -1)
		return -errno;

	return 0;
}

static int vigenere_chmod(const char *path, mode_t mode)
{
	int res;

	path = prepend_storage_dir(storage_path, path);
	res = chmod(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int vigenere_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;

	path = prepend_storage_dir(storage_path, path);
	res = lchown(path, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int vigenere_truncate(const char *path, off_t size)
{
	int res;

	path = prepend_storage_dir(storage_path, path);
	res = truncate(path, size);
	if (res == -1)
		return -errno;

	return 0;
}

#ifdef HAVE_UTIMENSAT
static int vigenere_utimens(const char *path, const struct timespec ts[2])
{
	int res;

	/* don't use utime/utimes since they follow symlinks */
	path = prepend_storage_dir(storage_path, path);
	res = utimensat(0, path, ts, AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		return -errno;

	return 0;
}
#endif

static int vigenere_open(const char *path, struct fuse_file_info *fi)
{
	int res;

	path = prepend_storage_dir(storage_path, path);
	res = open(path, fi->flags);
	if (res == -1)
		return -errno;

	close(res);

	return 0;
}

static int vigenere_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int fd;
	int res;
	int i;
	char temp_buf[size];

	(void) fi;
	//compute path(pointer to file whose data will be read)	
	path = prepend_storage_dir(storage_path, path);


	fd = open(path, O_RDONLY); //open file for reading
	if (fd == -1)//if unable to open file 
		return -errno;//return error number

	res = pread(fd, temp_buf, size, offset);//read file into temp_buff
	if (res == -1)//if unable to read
		res = -errno;//set returned result to error number

	//generate key given keyword
	if(key==""){
		char temp_key[size];
		for(int n=0; n<size; n++){
			temp_key[n] = keyword[n%keyword.size()];
		}
		key = temp_key
	}//if key is null 

	// Copy the read data from a temporary buffer with each character
	// (un)shifted.
	for (i = 0; i < size; i++) {
    	buf[i] = (temp_buf[i] - key[i % key_length] + 256) % 256;
	}//shift character by key to encrypt

	close(fd);
	return res;
}

static int vigenere_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	int i;
	char temp_buf[size];

	(void) fi;
	//compute path(pointer to file whose data will be written to)
	path = prepend_storage_dir(storage_path, path);
	fd = open(path, O_WRONLY);//open file for writing
	if (fd == -1)//if unable to open file 
		return -errno;//set return error number

	//generate key given keyword
	if(key==""){
		char temp_key[size];
		for(int n=0; n<size; n++){
			temp_key[n] = keyword[n%keyword.size()];
		}
		key = temp_key
	}//if key is null 
	
	// Copy the provided data into a temporary buffer with each character
	// shifted.
	for (i = 0; i < size; i++) {
    	temp_buf[i] = (buf[i] + key[i % key_length]) % 256;
	}


	res = pwrite(fd, temp_buf, size, offset);// Write data into file
	if (res == -1)//in unable to write data to file
		res = -errno;//set returned result to error number

	close(fd);
	return res;
}

static int vigenere_statfs(const char *path, struct statvfs *stbuf)
{
	int res;

	path = prepend_storage_dir(storage_path, path);
	res = statvfs(path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int vigenere_release(const char *path, struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) fi;
	return 0;
}

static int vigenere_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
static int vigenere_fallocate(const char *path, int mode,
			off_t offset, off_t length, struct fuse_file_info *fi)
{
	int fd;
	int res;

	(void) fi;

	if (mode)
		return -EOPNOTSUPP;

	path = prepend_storage_dir(storage_path, path);
	fd = open(path, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = -posix_fallocate(fd, offset, length);

	close(fd);
	return res;
}
#endif

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
static int vigenere_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
	path = prepend_storage_dir(storage_path, path);
	int res = lsetxattr(path, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int vigenere_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
	path = prepend_storage_dir(storage_path, path);
	int res = lgetxattr(path, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int vigenere_listxattr(const char *path, char *list, size_t size)
{
	path = prepend_storage_dir(storage_path, path);
	int res = llistxattr(path, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int vigenere_removexattr(const char *path, const char *name)
{
	path = prepend_storage_dir(storage_path, path);
	int res = lremovexattr(path, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations vigenere_oper = {
	.getattr	= vigenere_getattr,
	.access		= vigenere_access,
	.readlink	= vigenere_readlink,
	.readdir	= vigenere_readdir,
	.mknod		= vigenere_mknod,
	.mkdir		= vigenere_mkdir,
	.symlink	= vigenere_symlink,
	.unlink		= vigenere_unlink,
	.rmdir		= vigenere_rmdir,
	.rename		= vigenere_rename,
	.link		= vigenere_link,
	.chmod		= vigenere_chmod,
	.chown		= vigenere_chown,
	.truncate	= vigenere_truncate,
#ifdef HAVE_UTIMENSAT
	.utimens	= vigenere_utimens,
#endif
	.open		= vigenere_open,
	.read		= vigenere_read,
	.write		= vigenere_write,
	.statfs		= vigenere_statfs,
	.release	= vigenere_release,
	.fsync		= vigenere_fsync,
#ifdef HAVE_POSIX_FALLOCATE
	.fallocate	= vigenere_fallocate,
#endif
#ifdef HAVE_SETXATTR
	.setxattr	= vigenere_setxattr,
	.getxattr	= vigenere_getxattr,
	.listxattr	= vigenere_listxattr,
	.removexattr	= vigenere_removexattr,
#endif
};

int main(int argc, char *argv[])
{
	umask(0);
	if (argc < 4) {
	  fprintf(stderr,
		  "USAGE: %s <storage directory> <mount point> <vigenere shift> [ -d | -f | -s ]\n",
		  argv[0]);
	  return 1;
	}
	storage_dir = argv[1];
	char* mount_dir = argv[2];
	key = atoi(argv[3]);
	if (storage_dir[0] != '/' || mount_dir[0] != '/') {
	  fprintf(stderr, "ERROR: Directories must be absolute paths\n");
	  return 1;
	}
	fprintf(stderr,
		"DEBUG: Mounting %s at %s using key %d\n",
		storage_dir,
		mount_dir,
		key);
	int short_argc = argc - 2;
	char* short_argv[short_argc];
	short_argv[0] = argv[0];
	short_argv[1] = mount_dir;
	for (int i = 4; i < argc; i += 1) {
	  short_argv[i - 2] = argv[i];
	}
	return fuse_main(short_argc, short_argv, &vigenere_oper, NULL);
}
