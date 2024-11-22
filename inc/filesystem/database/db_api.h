#ifndef _DB_API_H_
#define _DB_API_H_

#ifdef __cplusplus
extern "C"{
#endif

#ifdef _WIN32
#include <stdio.h>
#include "assert.h"
#include "oal.h"
#define SDROOT	"C:\\sdcard2"
#else
#include "sys_inc.h"
#include "hal.h"
#define assert(...)
#define SDROOT	"0:"
#endif

#define DBROOT			SDROOT"\\DBROOT"
#define WAVEROOT		SDROOT"\\record"
#define EBROOT			DBROOT"\\ebook"
#define F_DBCONTENT		DBROOT"\\db_content.db"
#define F_DBAUDIO		DBROOT"\\db_audio.db"
#define F_ALBUM			DBROOT"\\db_album.db"
#define F_ARTIST		DBROOT"\\db_artist.db"
#define F_DBVIDEO		DBROOT"\\db_video.db"
#define F_DBIMAGE		DBROOT"\\db_image.db"
#define F_DBEBOOK		DBROOT"\\db_ebook.db"
#define F_DBWAVE		DBROOT"\\db_wave.db"
#define F_DBLIKE		DBROOT"\\db_like.db"
#define F_DBTAG			DBROOT"\\db_tag.db"
#define F_DBFOLDER0		DBROOT"\\db_folder0.db"
#define F_DBFOLDER1		DBROOT"\\db_folder1.db"
#define F_DBFOLDER2		DBROOT"\\db_folder2.db"
#define F_DBFOLDER3		DBROOT"\\db_folder3.db"
#define F_DBFOLDER4		DBROOT"\\db_folder4.db"
#define F_DBFOLDER5		DBROOT"\\db_folder5.db"
#define F_DBFOLDER6		DBROOT"\\db_folder6.db"
#define F_DBFOLDER7		DBROOT"\\db_folder7.db"
#define F_FPL			DBROOT"\\db_fpl.db"
#define F_SORT			DBROOT"\\db_sort.db"

#define	MEDIA_LO_TXT	(0x01<<0)		//	".txt",
#define	MEDIA_LO_BMP	(0x01<<1)		//	".bmp",
#define	MEDIA_LO_JPG	(0x01<<2)		//	".jpg",
#define	MEDIA_LO_JPEG	(0x01<<3)		//	".jpeg",
#define	MEDIA_LO_MP3	(0x01<<4)		//	".mp3",
#define	MEDIA_LO_AAC	(0x01<<5)		//	".aac",
#define	MEDIA_LO_WMA	(0x01<<6)		//	".wma",
#define	MEDIA_LO_M4A	(0x01<<7)		//	".m4a",
#define	MEDIA_LO_FLAC	(0x01<<8)		//	".flac",
#define	MEDIA_LO_APE	(0x01<<9)		//	".ape",
#define	MEDIA_LO_WAV	(0x01<<10)		//	".wav",
#define	MEDIA_LO_OGG	(0x01<<11)		//	".ogg",
#define	MEDIA_LO_AVI	(0x01<<12)		//	".avi",
#define MEDIA_HI_AUDIO	(0x01<<16)		//	AUDIO
#define MEDIA_HI_VIDEO	(0x01<<17)		//	VIDEO
#define MEDIA_HI_IMAGE	(0x01<<18)		//	IMAGE
#define MEDIA_HI_EBOOK	(0x01<<19)		//	TEXT
#define MEDIA_HI_WAVE	(0x01<<20)		//	WAVE file in record folder
#define MEDIA_HI_FOLDER	(0x01<<21)		//	FOLDER
#define MEDIA_ALL		0xFFFFFFFF

#define MODULE_AUDIO	(0x01<<0)
#define MODULE_ALBUM	(0x01<<1)
#define MODULE_ARTIST	(0x01<<2)
#define MODULE_VIDEO	(0x01<<3)
#define MODULE_IMAGE	(0x01<<4)
#define MODULE_EBOOK	(0x01<<5)
#define MODULE_WAVE		(0x01<<6)
#define MODULE_LIKE		(0x01<<7)
#define MODULE_TAG		(0x01<<8)
#define MODULE_FOLDER	(0x01<<9)
#define MODULE_ALL		0x000003FF

#define NEST			8

#define	MAXPATH			260
#define MAXUTF8NAME		64
#define MAXID3ITEM		16
#define MAXRECORD		5000
#define MAXLIKETYPE		6
#define MAXLIKERECORD	1024
#define MAXTAG			5

#define SORT			1

typedef void (*db_cb)(int state, int code);	//db_cb只在build调用，目的是驱动进度条和报告错误，
											//state==0，code代表错误码，
											//state==-555，code代表进度百分比,
											//state==-666，代表build完成，不理会code
											//state==-777，SD卡坏了需要修复，不理会code
											//state==-888，OAL_malloc失败，不理会code

typedef int (*get_sd_status)(int sd_id);
typedef char RECUTF8NAME[MAXUTF8NAME];
typedef char RECID3ITEM[MAXID3ITEM];

typedef struct tagAudioRecord
{
	char path[MAXPATH];			//数据库主键，不能修改，不能转码
	char fname[MAXUTF8NAME];	//utf8格式的文件名，只用于UI显示
	char album[MAXID3ITEM];		//utf8格式
	char artist[MAXID3ITEM];	//utf8格式
}AUDIORECORD;

typedef struct tagVideoRecord
{
	char path[MAXPATH];			//数据库主键，不能修改，不能转码
	char fname[MAXUTF8NAME];	//utf8格式的文件名，只用于UI显示
}VIDEORECORD, IMAGERECORD, WAVERECORD, LIKERECORD;

typedef struct tagEBookRecord
{
	char path[MAXPATH];			//数据库主键，不能修改，不能转码
	char fname[MAXUTF8NAME];	//utf8格式的文件名，只用于UI显示
	unsigned int ebookSN;		//ebook流水号
}EBOOKRECORD;

typedef struct tagFolderRecord
{
	char path[MAXPATH];			//数据库主键，不能修改，不能转码
	char fname[MAXUTF8NAME];	//utf8格式的文件名，只用于UI显示
	int media_type;				//记录的媒体类别；
}FOLDERRECORD;

typedef struct tagTagRecord
{
	char path[MAXPATH];			//数据库主键，不能修改，不能转码
	unsigned int tag[MAXTAG];	//标签值；
}TAGRECORD;

int db_init(db_cb cb, get_sd_status cb_get_sd_status);//返回0=成功；-99=卡满；others=文件系统错误
void db_free(void);
int db_mount(void);
int db_unmount(void);
int db_is_dirty(int module);		//返回1=该module脏，需要重新扫描。module子模块包括MODULE_AUDIO到MODULE_TAG

int db_audio_build(const char * ignore_dir[]);
int db_album_build(void);
int db_artist_build(void);
int db_get_audio_count(void);
int db_get_album_count(void);
int db_get_artist_count(void);

int db_get_audio_records(			// 获取音乐文件记录
	unsigned int offset,			// 偏移值
	AUDIORECORD* records,			// 输出的记录指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_audio_name(				// 获取音乐文件记录
	unsigned int offset,			// 偏移值
	RECUTF8NAME* fname,				// 输出的utf8文件名指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_albums(					// 获取专辑名称
	unsigned int offset,			// 偏移值
	RECID3ITEM* albums,				// 输出的utf8专辑名指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_artists(					// 获取艺术家名称
	unsigned int offset,			// 偏移值
	RECID3ITEM* artists,			// 输出的utf8专辑名指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_audio_count_of_album(	// 获取特定专辑的音乐数目
	RECID3ITEM album				// 输出的utf8专辑名，非空
);									// 返回特定专辑的音乐数目，-1表示出错；

int db_get_audio_count_of_artist(	// 获取特定艺术家的音乐数目
	RECID3ITEM artist				// 输出的utf8艺术家名，非空
);									// 返回特定艺术家的音乐数目，-1表示出错；

int db_get_audio_name_by_album(		// 获取专辑下的音乐文件名称
	char* album,					// 输入的utf8专辑名指针，非空
	unsigned int offset,			// 偏移值
	RECUTF8NAME* fname,				// 输出的utf8文件名指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_audio_name_by_artist(	// 获取艺术家下的音乐文件名称
	char* artist,					// 输入的utf8艺术家，非空
	unsigned int offset,			// 偏移值
	RECUTF8NAME* fname,				// 输出的utf8文件名指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_audio_record_by_album(	// 获取专辑下的音乐文件记录信息
	char* album,					// 输入的utf8专辑名，非空
	unsigned int offset,			// 偏移值
	AUDIORECORD* records,			// 输出的记录指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_audio_record_by_artist(	// 获取艺术家下的音乐文件记录信息
	char* artist,					// 输入的utf8艺术家名，非空
	unsigned int offset,			// 偏移值
	AUDIORECORD* records,			// 输出的记录指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_video_build(const char * ignore_dir[]);
int db_get_video_count(void);

int db_get_video_records(			// 获取视频文件记录
	unsigned int offset,			// 偏移值
	VIDEORECORD* records,			// 输出的记录指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_video_name(				// 获取视频文件名
	unsigned int offset,			// 偏移值
	RECUTF8NAME* fname,				// 输出的utf8文件名指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_image_build(const char * ignore_dir[]);
int db_get_image_count(void);

int db_get_image_records(			// 获取图片文件记录
	unsigned int offset,			// 偏移值
	IMAGERECORD* records,			// 输出的记录指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_image_name(				// 获取图片文件名
	unsigned int offset,			// 偏移值
	RECUTF8NAME* fname,				// 输出的utf8文件名指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_ebook_build(const char * ignore_dir[]);
int db_get_ebook_count(void);

int db_get_ebook_records(			// 获取电子书文件记录
	unsigned int offset,			// 偏移值
	EBOOKRECORD* records,			// 输出的记录指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_ebook_name(				// 获取电子书文件名
	unsigned int offset,			// 偏移值
	RECUTF8NAME* fname,				// 输出的utf8文件名指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_ebookSN(					// 查询给定text文件的ebook流水号
	char* path,						// 输入的text文件路径，非空
	unsigned int* ebookSN			// 返回，ebook流水号
);									// 返回-1表示出错；0=OK

int db_wave_build(void);
int db_get_wave_count(void);

int db_get_wave_records(			// 获取录音文件记录
	unsigned int offset,			// 偏移值
	WAVERECORD* records,			// 输出的记录指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_wave_name(				// 获取录音文件名
	unsigned int offset,			// 偏移值
	RECUTF8NAME* fname,				// 输出的utf8文件名指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_add_wave(char* path);		// 返回-1表示出错；0=OK

int db_enter_folder(char* dir);
int db_get_count_of_folder(char* dir);
int db_get_folder_records(			// 获取目录下的文件记录
	char* dir,						// 输入目录，非空
	unsigned int offset,			// 偏移值
	FOLDERRECORD* records,			// 输出的记录指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_build_fpl(char* dir);

int db_get_count_of_fpl(char* dir);

int db_get_fpl_records(				// 获取某个目录下音乐记录
	char* dir,						// 输入目录，非空
	unsigned int offset,			// 偏移值
	FOLDERRECORD* records,			// 输出的记录指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_fpl_name(				// 获取某个目录下音乐的文件名
	char* dir,						// 输入目录，非空
	unsigned int offset,			// 偏移值
	RECUTF8NAME* fname,				// 输出的utf8文件名指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_fpl_index(				// 获取某个音乐文件的fpl索引序号
	char* path						// 输入的文件路径，非空
);									// 返回fpl索引序号，-1表示出错；

int db_delete_file(char* path);		// 返回-1表示出错；0=OK

int db_get_audio_id3(				// 解析给定文件的Id3信息
	char* path,						// 输入的文件路径，非空
	AUDIORECORD* record				// 输出的记录指针，非空
);									// 返回0==成功；1==解析失败

int db_like_build(void);
int db_get_like_count(int like);	// like 取值[1, MAXLIKETYPE]

int db_get_like_records(			// 获取目录下的文件记录
	int like,						// like 取值[1, MAXLIKETYPE]
	unsigned int offset,			// 偏移值
	LIKERECORD* records,			// 输出的记录指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_get_like_name(				// 获取某个喜欢列表中的文件名
	int like,						// like 取值[1, MAXLIKETYPE]
	unsigned int offset,			// 偏移值
	RECUTF8NAME* fname,				// 输出的utf8文件名指针，非空
	unsigned int cnt				// 请求的记录个数
);									// 返回读取到的记录个数，-1表示出错；

int db_is_like(						// 查询指定文件的like向量，1==喜欢；0==非喜欢
	char* path,						// 输入指定的文件，非空
	int* like						// 返回的like向量，按位表示，0 != (like &（1<<b)) 代表第b位非零，即第（b+1)个like类被设置了。
);									// 返回 0==成功；-1==失败

int db_set_like(					// 设置指定文件为like
	char* path,						// 输入指定的文件，非空
	int like						// like值必须在±MAXLIKETYPE之间，不能为0！正值表示设置为喜欢；负值表示设置成不喜欢
);									// 返回 0==成功；-1==失败

int db_tag_build(void);
int db_get_tag_count(void);

int db_get_tag_record(				// 获取指定文件的tag信息
	char* path,						// 输入文件名，非空
	TAGRECORD* record				// 输出的记录指针，非空
	);								// 返回读取到的记录个数，-1表示出错；

int db_add_tag(
	char* path,						// 文件名；类型是 MEDIA_HI_TEXT / MEDIA_HI_AUDIO / MEDIA_HI_WAVE
	unsigned int No,				// 标签序号
	unsigned int tag				// 标签值
	);								// 返回-1表示出错；0=OK

int db_reset(void);  // 数据库清除所有。

#ifdef __cplusplus
}
#endif

#endif
