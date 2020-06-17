/* *********************************************************************
 * Copyright (C) 2007-2015 VMware, Inc. All Rights Reserved. -- VMware Confidential
 * *********************************************************************/

 /*
  * This demonstrates how to set up and use guest operations.
  * This assumes that the guest OS is Linux.
  *
  * It executes 'ls' on the guest OS, redirecting it to a file.
  * It then copies that output file to the host.
  * Finally it cleans up on the guest.
  */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "vix.h"

  /*
   * guest OS username/password configs
   */
#define  GUEST_VMXFILE  "VMXFILE"
#define  GUEST_USERNAME "USERNAME"
#define  GUEST_PASSWORD "PASSWORD"
#define	 GUEST_OUTPATH  "OUTPATH"
   /*
	* Certain arguments differ when using VIX with VMware Server 2.0
	* and VMware Workstation.
	*
	* Comment out this definition to use this code with VMware Server 2.0.
	*/
#define  USE_WORKSTATION

#ifdef USE_WORKSTATION

#define  CONNTYPE    VIX_SERVICEPROVIDER_VMWARE_WORKSTATION

#define  HOSTNAME ""
#define  HOSTPORT 0
#define  USERNAME ""
#define  PASSWORD ""

#define  VMPOWEROPTIONS   VIX_VMPOWEROP_LAUNCH_GUI

#define  CFG_INFO "where cfg is an absolute path to the .cfg file " \
                  "comprising key-value pairs."

#else    // USE_WORKSTATION

	/*
	 * For VMware Server 2.0
	 */

#define  CONNTYPE VIX_SERVICEPROVIDER_VMWARE_VI_SERVER

#define  HOSTNAME "https://192.2.3.4:8333/sdk"
	 /*
	  * NOTE: HOSTPORT is ignored, so the port should be specified as part
	  * of the URL.
	  */
#define  HOSTPORT 0
#define  USERNAME "root"
#define  PASSWORD "hideme"

#define  VMPOWEROPTIONS VIX_VMPOWEROP_NORMAL

#define  CFG_INFO "where cfg is an absolute path to the .cfg file " \
                  "comprising key-value pairs."

#endif    // USE_WORKSTATION

#define  TOOLS_TIMEOUT  300

#ifdef _WIN32
#define  DEST_FILE "./outfile.txt"
#define  BUFF_SIZE 128
#else
#define  DEST_FILE "./outfile.txt"
#endif

static char *progname;

static struct kv_node {
	char *key;
	char *value;
	struct kv_node *next;
} *configs = NULL;

static int
add_config(const char *key, const char *value)
{
	struct kv_node **ptr = &configs;
	struct kv_node *tmp_node;
	char *tmp_str;

	while (*ptr && strcmp((*ptr)->key, key) < 0)            // traverse nodes
		ptr = &(*ptr)->next;

	if (!(*ptr) || strcmp((*ptr)->key, key) > 0) {          // insert new node
		tmp_node = *ptr;
		if (!(*ptr = malloc(sizeof(struct kv_node)))) {
			*ptr = tmp_node;
			return -1;
		}
		if (!((*ptr)->key = malloc(sizeof(char)*(strlen(key) + 1)))) {
			free(*ptr);
			*ptr = tmp_node;
			return -1;
		}
		if (strcpy_s((*ptr)->key, strlen(key) + 1, key)) {  // set key
			free((*ptr)->key);
			free(*ptr);
			*ptr = tmp_node;
			return -1;
		}
		(*ptr)->value = NULL;
		(*ptr)->next = tmp_node;
	}

	tmp_str = (*ptr)->value;
	if (!((*ptr)->value = malloc(sizeof(char)*(strlen(value) + 1)))) {
		(*ptr)->value = tmp_str;
		return -1;
	}
	if (strcpy_s((*ptr)->value, strlen(value) + 1, value)) {// set value
		free((*ptr)->value);
		(*ptr)->value = tmp_str;
		return -1;
	}
	if (tmp_str) free(tmp_str);
	return 0;
}

static char*
get_config(const char* key)
{
	struct kv_node *tmp_node = configs;
	while (tmp_node && strcmp(tmp_node->key, key) < 0)
		tmp_node = tmp_node->next;
	return (tmp_node && strcmp(tmp_node->key, key) == 0) ?
		tmp_node->value : NULL;
}

static void
free_config()
{
	struct kv_node *tmp_node;
	while (configs) {
		tmp_node = configs;
		configs = configs->next;
		if (tmp_node->value) free(tmp_node->value);
		if (tmp_node->key) free(tmp_node->key);
		free(tmp_node);
	}
}

static char *
trim(char *str)
{
	char *end;
	/* skip leading whitespace */
	while (isspace(*str)) ++str;
	/* remove trailing whitespace */
	end = str + strlen(str) - 1;
	while (end > str && isspace(*end)) --end;
	/* write null character */
	*(end + 1) = '\0';
	return str;
}

const char *
concat_vmx(const char *clone_name) {
	char * str2 = ".vmx";
	int count = 0, c = 0;
	char *tmp = clone_name;
	while (tmp[count] != '\0')
		count++;
	while (str2[c] != '\0')
		tmp[count++] = str2[c++];
	tmp[count] = '\0';
	return tmp;
}

const char *
create_directory(char *dirname, const char *clone_name) {
	struct stat st = { 0 };
	char * path = "clone_dir\\", *dir;
	char * full_dirname = malloc(1 + strlen(path) + strlen(dirname));
	int count = 0, b = 0;
	while (full_dirname[count] != '\0') {
		count++;
	}
	while (path[b] != '\0')
		full_dirname[count++] = path[b++];
	b = 0;
	while (dirname[b] != '\0')
		full_dirname[count++] = dirname[b++];
	b = 0;
	full_dirname[count] = '\0';
	if (stat(full_dirname, &st) == -1) {
		mkdir(full_dirname);
	}
	full_dirname[count++] = '\\';
	while (clone_name[b] != '\0')
		full_dirname[count++] = clone_name[b++];
	full_dirname[count] = '\0';
	return full_dirname;
}

static int
read_fcfg(const char *cfg)
{
	FILE *fl_cfg;
	char buff[BUFF_SIZE], key[BUFF_SIZE], value[BUFF_SIZE];
	char *e_pos;
	int lineNo = 0, err = 0;

	if (fopen_s(&fl_cfg, cfg, "rt")) {
		fprintf(stderr, "Failed to read file!\n");
		err = -1;
	}
	while (!err && !feof(fl_cfg)) {
		lineNo++;
		if (fgets(buff, BUFF_SIZE, fl_cfg) && (e_pos = strchr(buff, '='))) {
			*e_pos = '\0';
			if (strcpy_s(key, BUFF_SIZE, trim(buff)) ||
				strcpy_s(value, BUFF_SIZE, trim(e_pos + 1))) {
				err = -1;
			}
			else {
				if (add_config(key, value)) {
					fprintf(stderr, "Config line %d: Out of memory!\n", lineNo);
					err = -1;
				}
			}
		}
		else if (!feof(fl_cfg) && strcmp(trim(buff), "") != 0) {
			fprintf(stderr, "Config line %d: Failed to parse!\n", lineNo);
			err = -1;
		}
	}
	if (fl_cfg) fclose(fl_cfg);
	return err;
}


char *get_file_name(char* myStr) {
	char *retStr;
	char *lastExt;
	if (myStr == NULL) return "Clone_Dir";
	if ((retStr = malloc(strlen(myStr) + 1)) == NULL) return "Clone_Dir";
	int count = 0;
	while (count < strlen(myStr)) {
		retStr[count] = myStr[count];
		count++;
	}
	retStr[count] = '\0';
	lastExt = strrchr(retStr, '.');
	if (lastExt != NULL) {
		*lastExt = '\0';
	}
	return retStr;
}


const char *get_filename_ext(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) return "";
	return dot;
}


static void
usage()
{
	fprintf(stderr, "Usage: %s <cfg>\n", progname);
	fprintf(stderr, "%s\n", CFG_INFO);
}

int
main(int argc, char **argv)
{
	char        *vmpath, *clone_folder, *filename;
	char		*clone_name = "My_VM_clone.vmx";
	char		*dirname = "My_VM_Clone_Dir";
	VixError    err;
	VixHandle   hostHandle = VIX_INVALID_HANDLE;
	VixHandle   jobHandle = VIX_INVALID_HANDLE;
	VixHandle   vmHandle = VIX_INVALID_HANDLE;
	VixHandle	cloneVMHandle = VIX_INVALID_HANDLE;
	VixHandle snapshotHandle = VIX_INVALID_HANDLE;
	progname = argv[0];
	if (argc > 1) {
		if (read_fcfg("./guest.cfg")) exit(EXIT_FAILURE);         // read config file
		vmpath = get_config(GUEST_VMXFILE);                 // get vmx file
		if (strcmp(argv[1], "FCLONE") == 0 || strcmp(argv[1], "LCLONE") == 0) {
			char *file_ext;
			if ((file_ext = strrchr(argv[2], '.')) != NULL) {
				if (strcmp(file_ext, ".vmx") == 0) {
					//ends with vmx then just put the argument to clone_name variable;
					clone_name = argv[2];
					printf("Nama Clone : %s\n", clone_name);
				}
				else {
					printf("file extension name not .vmx setting to default");
				}
			}
			else {
				clone_name = concat_vmx(argv[2]);
				printf("Nama Clone : %s\n", clone_name);
			}

		}
	}
	else {
		usage();
		exit(EXIT_FAILURE);
	}

	jobHandle = VixHost_Connect(VIX_API_VERSION,          // api version
		CONNTYPE,                 // connection type
		HOSTNAME,                 // host name
		HOSTPORT,                 // host port
		USERNAME,                 // username
		PASSWORD,                 // passwd
		0,                        // options
		VIX_INVALID_HANDLE,       // property list handle
		NULL,                     // callback
		NULL);                    // client data

	err = VixJob_Wait(jobHandle, VIX_PROPERTY_JOB_RESULT_HANDLE,
		&hostHandle, VIX_PROPERTY_NONE);
	Vix_ReleaseHandle(jobHandle);
	if (VIX_FAILED(err)) {
		fprintf(stderr, "failed to connect to host (%"FMT64"d %s)\n", err,
			Vix_GetErrorText(err, NULL));
		goto abort;
	}
	printf("connected to host (%d)\n", hostHandle);

	printf("about to open %s\n", vmpath);
	jobHandle = VixVM_Open(hostHandle,               // host connection
		vmpath,                   // path to vmx
		NULL,                     // callback
		NULL);                    // client data
	err = VixJob_Wait(jobHandle, VIX_PROPERTY_JOB_RESULT_HANDLE,
		&vmHandle, VIX_PROPERTY_NONE);
	Vix_ReleaseHandle(jobHandle);
	if (VIX_FAILED(err)) {
		fprintf(stderr, "failed to open virtual machine '%s'(%"FMT64"d %s)\n", vmpath, err,
			Vix_GetErrorText(err, NULL));
		goto abort;
	}
	printf("opened %s (%d)\n", vmpath, vmHandle);

	//TASKS
	if (strcmp(argv[1], "START") == 0) {
		printf("powering on\n");
		jobHandle = VixVM_PowerOn(vmHandle,                 // vm handle
			VMPOWEROPTIONS,           // options
			VIX_INVALID_HANDLE,       // property list
			NULL,                     // callback
			NULL);                    // client data
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		Vix_ReleaseHandle(jobHandle);
		if (VIX_FAILED(err)) {
			fprintf(stderr, "failed to power on virtual machine (%"FMT64"d %s)\n", err,
				Vix_GetErrorText(err, NULL));
			goto abort;
		}
		printf("powered on\n");
	}
	else if (strcmp(argv[1], "SUSPEND") == 0) {
		Vix_ReleaseHandle(jobHandle);
		jobHandle = VixVM_Suspend(vmHandle,
			VIX_VMPOWEROP_NORMAL,
			NULL, // *callbackProc,
			NULL); // *clientData);
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		if (VIX_FAILED(err)) {
			goto abort;
		}

		Vix_ReleaseHandle(jobHandle);
	}
	else if (strcmp(argv[1], "STOP") == 0) {
		jobHandle = VixVM_PowerOff(vmHandle,
			VIX_VMPOWEROP_NORMAL,
			NULL, // *callbackProc,
			NULL); // *clientData);
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		if (VIX_FAILED(err)) {
			goto abort;
			printf("Error turning off the VM");
		}
		Vix_ReleaseHandle(jobHandle);
	}
	else if (strcmp(argv[1], "SNAPSHOT") == 0) {
		// Create snapshot in powered-on state.
		jobHandle = VixVM_CreateSnapshot(vmHandle,
			argv[2], // name
			argv[3], // description
			VIX_SNAPSHOT_INCLUDE_MEMORY, // options
			VIX_INVALID_HANDLE, // propertyListHandle
			NULL, // callbackProc
			NULL); // clientData
		err = VixJob_Wait(jobHandle,
			VIX_PROPERTY_JOB_RESULT_HANDLE,
			&snapshotHandle,
			VIX_PROPERTY_NONE);
		Vix_ReleaseHandle(jobHandle);
		jobHandle = VixVM_PowerOff(vmHandle,
			VIX_VMPOWEROP_NORMAL,
			NULL, // *callbackProc,
			NULL); // *clientData);
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		if (VIX_FAILED(err)) {
			goto abort;
			printf("Error turning off the VM");
		}
		Vix_ReleaseHandle(jobHandle);
		printf("snapshot success..");
	}
	else if (strcmp(argv[1], "REVERTSNAPSHOT") == 0) {
		// Revert to snapshot #0.
		const char *ssname = argv[2];
		err = VixVM_GetNamedSnapshot(vmHandle,
			ssname,
			&snapshotHandle);
		int numRootSnapShots;
		/*VixVM_GetNumRootSnapshots(vmHandle, &numRootSnapShots);
		printf("jumlah :%d", numRootSnapShots);*/
		if (VIX_OK != err) {
			// Handle the error...
			goto abort;
		}
		jobHandle = VixVM_RevertToSnapshot(vmHandle,
			snapshotHandle,
			VIX_VMPOWEROP_LAUNCH_GUI, // options
			VIX_INVALID_HANDLE, // propertyListHandle
			NULL, // callbackProc
			NULL); // clientData

		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);

		if (VIX_OK != err) {
			// Handle the error...
			goto abort;
		}
		Vix_ReleaseHandle(jobHandle);
	}
	else if (strcmp(argv[1], "FCLONE") == 0 && clone_name != NULL)
	{
		filename = clone_name;
		dirname = get_file_name(filename);
		clone_folder = create_directory(dirname, filename);
		printf("Clone folder path: %s", clone_folder);
		jobHandle = VixVM_Clone(vmHandle,
			VIX_INVALID_HANDLE,  // snapshotHandle
			VIX_CLONETYPE_FULL,  // cloneType
			clone_folder,  // destConfigPathName
			0,  //options,
			VIX_INVALID_HANDLE,  // propertyListHandle
			NULL,  // callbackProc
			NULL);  // clientData
		err = VixJob_Wait(jobHandle,
			VIX_PROPERTY_JOB_RESULT_HANDLE,
			&cloneVMHandle,
			VIX_PROPERTY_NONE);
		if (VIX_FAILED(err)) {
			// Handle the error...
			goto abort;
		}
		Vix_ReleaseHandle(jobHandle);
	}
	else if (strcmp(argv[1], "LCLONE") == 0 && clone_name != NULL) //kondisi vm harus mati dulu
	{
		filename = clone_name;
		dirname = get_file_name(filename);
		clone_folder = create_directory(dirname, filename);
		printf("Clone folder path: %s", clone_folder);
		jobHandle = VixVM_Clone(vmHandle,
			VIX_INVALID_HANDLE,  // snapshotHandle
			VIX_CLONETYPE_LINKED,  // cloneType
			clone_folder,  // destConfigPathName
			0,  //options,
			VIX_INVALID_HANDLE,  // propertyListHandle
			NULL,  // callbackProc
			NULL);  // clientData
		err = VixJob_Wait(jobHandle,
			VIX_PROPERTY_JOB_RESULT_HANDLE,
			&cloneVMHandle,
			VIX_PROPERTY_NONE);
		if (VIX_FAILED(err)) {
			// Handle the error...
			goto abort;
		}
		Vix_ReleaseHandle(jobHandle);
	}
	else if (strcmp(argv[1], "RUNPROGRAM") == 0) {
		printf("waiting for tools\n");
		jobHandle = VixVM_WaitForToolsInGuest(vmHandle,
			TOOLS_TIMEOUT,     // timeout in secs
			NULL,              // callback
			NULL);             // client data
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		Vix_ReleaseHandle(jobHandle);
		if (VIX_FAILED(err)) {
			fprintf(stderr, "failed to wait for tools in virtual machine (%"FMT64"d %s)\n", err,
				Vix_GetErrorText(err, NULL));
			goto abort;
		}
		printf("tools up\n");

		jobHandle = VixVM_LoginInGuest(vmHandle,
			get_config(GUEST_USERNAME),  // get guest OS user
			get_config(GUEST_PASSWORD),  // get guest OS passwd
			0,                           // options
			NULL,                        // callback
			NULL);                       // client data
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		Vix_ReleaseHandle(jobHandle);
		if (VIX_FAILED(err)) {
			fprintf(stderr, "failed to login to virtual machine '%s'(%"FMT64"d %s)\n", vmpath, err,
				Vix_GetErrorText(err, NULL));
			goto abort;
		}
		printf("logged in to guest\n");
		printf("about to do work\n");
		jobHandle = VixVM_RunProgramInGuest(vmHandle,
			argv[2],                // command
			argv[3],   // cmd args
			0,                        // options
			VIX_INVALID_HANDLE,       // prop handle
			NULL,                     // callback
			NULL);                    // client data
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		Vix_ReleaseHandle(jobHandle);
		if (VIX_FAILED(err)) {
			fprintf(stderr, "failed to run program in virtual machine '%s'(%"FMT64"d %s)\n",
				vmpath, err, Vix_GetErrorText(err, NULL));
			goto abort;
		}

		jobHandle = VixVM_CopyFileFromGuestToHost(vmHandle,
			get_config(GUEST_OUTPATH),     // src file
			DEST_FILE,          // dst file
			0,                  // options
			VIX_INVALID_HANDLE, // prop list
			NULL,               // callback
			NULL);              // client data
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		Vix_ReleaseHandle(jobHandle);
		if (VIX_FAILED(err)) {
			fprintf(stderr, "failed to copy file to the host '%s'(%"FMT64"d %s)\n",
				vmpath, err, Vix_GetErrorText(err, NULL));
			goto abort;
		}
		FILE *hasil_running;
		int chr = 0;
		if ((err = fopen_s(&hasil_running, DEST_FILE, "r")) == 0)
		{

			printf("Hasil Running Program :\n");
			while ((chr = getc(hasil_running)) != EOF)
			{
				printf("%c", chr);
			}
			fclose(hasil_running);
		}
		else
		{
			fprintf(stderr, "Cannot open file, error %d\n", err);
			// handle the error further if needed
		}
	}
	else if (strcmp(argv[1], "SCRIPT") == 0) {
		printf("waiting for tools\n");
		jobHandle = VixVM_WaitForToolsInGuest(vmHandle,
			TOOLS_TIMEOUT,     // timeout in secs
			NULL,              // callback
			NULL);             // client data
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		Vix_ReleaseHandle(jobHandle);
		if (VIX_FAILED(err)) {
			fprintf(stderr, "failed to wait for tools in virtual machine (%"FMT64"d %s)\n", err,
				Vix_GetErrorText(err, NULL));
			goto abort;
		}
		printf("tools up\n");

		jobHandle = VixVM_LoginInGuest(vmHandle,
			get_config(GUEST_USERNAME),  // get guest OS user
			get_config(GUEST_PASSWORD),  // get guest OS passwd
			0,                           // options
			NULL,                        // callback
			NULL);                       // client data
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		Vix_ReleaseHandle(jobHandle);
		if (VIX_FAILED(err)) {
			fprintf(stderr, "failed to login to virtual machine '%s'(%"FMT64"d %s)\n", vmpath, err,
				Vix_GetErrorText(err, NULL));
			goto abort;
		}
		printf("logged in to guest\n");
		printf("about to do work\n");
		errno_t err;
		FILE *user_file;
		int chr = 0, count = 0;
		char *scripttext;
		// baca script -> dimasukan pointer -> dirun di script guest 
		/*printf("%s\n", get_filename_ext(argv[2]));*/
		if ((err = fopen_s(&user_file, argv[2], "r")) == 0)
		{
			scripttext = malloc(10000);
			while ((chr = getc(user_file)) != EOF)
			{

				scripttext[count++] = (char)chr;
			}
			scripttext[count] = '\0';
			fclose(user_file);
		}
		else
		{
			fprintf(stderr, "Cannot open file, error %d\n", err);
			scripttext = "";
			// handle the error further if needed
		}
		//// Run the target program.
		jobHandle = VixVM_RunScriptInGuest(vmHandle,
			get_config(get_filename_ext(argv[2])),
			scripttext,
			0, // options,
			VIX_INVALID_HANDLE, // propertyListHandle,
			NULL, // callbackProc,
			NULL); // clientData

		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		char *time_lapse;
		Vix_GetProperties(jobHandle, VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_ELAPSED_TIME, &time_lapse);
		printf("Waktu running script : %ds\n", time_lapse);
		if (VIX_OK != err) {
			// Handle the error...
			goto abort;
		}
		Vix_ReleaseHandle(jobHandle);

		jobHandle = VixVM_CopyFileFromGuestToHost(vmHandle,
			get_config(GUEST_OUTPATH),     // src file
			DEST_FILE,          // dst file
			0,                  // options
			VIX_INVALID_HANDLE, // prop list
			NULL,               // callback
			NULL);              // client data
		err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
		Vix_ReleaseHandle(jobHandle);
		if (VIX_FAILED(err)) {
			fprintf(stderr, "failed to copy file to the host '%s'(%"FMT64"d %s)\n",
				vmpath, err, Vix_GetErrorText(err, NULL));
			goto abort;
		}
		FILE *hasil_script;
		if ((err = fopen_s(&hasil_script, DEST_FILE, "r")) == 0)
		{

			printf("Hasil Script :\n");
			while ((chr = getc(hasil_script)) != EOF)
			{
				printf("%c", chr);
			}
			fclose(hasil_script);
		}
		else
		{
			fprintf(stderr, "Cannot open file, error %d\n", err);
			scripttext = "";
			// handle the error further if needed
		}


	}

	else {
		printf("Command/Argument yang diberikan salah atau kurang !");
	}
abort:
	Vix_ReleaseHandle(vmHandle);
	VixHost_Disconnect(hostHandle);
	free_config();                                          // free configs

	return 0;
}




//if (strcmp(argv[1], "LIST") == 0) {
//	jobHandle = VixVM_ListDirectoryInGuest(vmHandle, argv[2], 0, NULL, NULL);
//	err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
//	if (VIX_OK != err) {
//		fprintf(stderr, "failed to list dir '%s' in vm '%s'(%"FMT64"d %s)\n",
//			argv[3], vmpath, err, Vix_GetErrorText(err, NULL));
//		goto abort;
//	}
//
//	int num = VixJob_GetNumProperties(jobHandle, VIX_PROPERTY_JOB_RESULT_ITEM_NAME);
//	printf("List Files : \n");
//	for (int i = 0; i < num; i++) {
//		char *fname;
//
//		err = VixJob_GetNthProperties(jobHandle,
//			i,
//			VIX_PROPERTY_JOB_RESULT_ITEM_NAME,
//			&fname,
//			VIX_PROPERTY_NONE);
//
//		printf("%s\n", i, fname);
//
//		Vix_FreeBuffer(fname);
//	}
//
//	Vix_ReleaseHandle(jobHandle);
//}
//else if (strcmp(argv[1], "MKDIR") == 0) {
//	jobHandle = VixVM_CreateDirectoryInGuest(vmHandle, argv[2], 0, NULL, NULL);
//	err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
//	if (VIX_OK != err) {
//		fprintf(stderr, "failed to make dir '%s' in vm '%s'(%"FMT64"d %s)\n",
//			argv[3], vmpath, err, Vix_GetErrorText(err, NULL));
//		goto abort;
//	}
//	printf(argv[2]);
//	printf(" created");
//	Vix_ReleaseHandle(jobHandle);
//}
//
//else if (strcmp(argv[1], "DELETEDIR") == 0) {
//	jobHandle = VixVM_DeleteDirectoryInGuest(vmHandle, argv[2], 0, NULL, NULL);
//	err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
//	if (VIX_OK != err) {
//		fprintf(stderr, "failed to delete dir '%s' in vm '%s'(%"FMT64"d %s)\n",
//			argv[3], vmpath, err, Vix_GetErrorText(err, NULL));
//		goto abort;
//	}
//	printf(argv[2]);
//	printf(" deleted");
//	Vix_ReleaseHandle(jobHandle);
//}
//
//else if (strcmp(argv[1], "RENAMEDIR") == 0) {
//	jobHandle = VixVM_RenameFileInGuest(vmHandle,
//		argv[2],
//		argv[3],
//		0,
//		0,
//		NULL,
//		NULL);
//	err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
//	if (VIX_OK != err) {
//		fprintf(stderr, "failed to rename dir '%s' in vm '%s'(%"FMT64"d %s)\n",
//			argv[3], vmpath, err, Vix_GetErrorText(err, NULL));
//		goto abort;
//	}
//	printf(argv[2]);
//	printf("renamed to");
//	printf(argv[3]);
//	Vix_ReleaseHandle(jobHandle);
//}
//
//else if (strcmp(argv[1], "REMOVEFILE") == 0) {
//	jobHandle = VixVM_DeleteFileInGuest(vmHandle,
//		argv[2],
//		NULL,
//		NULL);
//	err = VixJob_Wait(jobHandle, VIX_PROPERTY_NONE);
//	if (VIX_OK != err) {
//		fprintf(stderr, "failed to delete dir '%s' in vm '%s'(%"FMT64"d %s)\n",
//			argv[3], vmpath, err, Vix_GetErrorText(err, NULL));
//		goto abort;
//	}
//	printf(argv[2]);
//	printf("deleted");
//	Vix_ReleaseHandle(jobHandle);
//}