// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#define INSTANCE_NAME_LENGTH (60+1)

extern unsigned short instance_start_id;
extern unsigned short instance_count;

/**
 * true if instance is in an active/playable state.
 * In other words, if a player can interact with it.
 *
 * @param inst instance_data to be checked
 */
#define instance_is_active(inst) ((inst).state == INSTANCE_IDLE || (inst).state == INSTANCE_BUSY)

typedef enum instance_state {
	INSTANCE_FREE,
	INSTANCE_IDLE,
	INSTANCE_BUSY,
	INSTANCE_DESTROYING,
} instance_state;

enum e_instance_mode {
	IM_NONE,
	IM_CHAR,
	IM_PARTY,
	IM_GUILD,
	IM_CLAN,
	IM_MAX,
};

enum e_instance_enter : uint8 {
	IE_OK,
	IE_NOMEMBER,
	IE_NOINSTANCE,
	IE_OTHER
};

struct instance_data {
	unsigned short id;
	char name[INSTANCE_NAME_LENGTH]; // Instance Name - required for clif functions.
	instance_state state;
	enum e_instance_mode mode;
	int owner_id;

	unsigned short *map;
	unsigned short num_map;
	unsigned short users;

	struct DBMap* vars; // Instance Variable for scripts
	
	int progress_timer;
	unsigned int progress_timeout;

	unsigned int original_progress_timeout;

	int idle_timer;
	unsigned int idle_timeout, idle_timeoutval;

	struct point respawn;/* reload spawn */
};

extern struct instance_data* instances;

bool instance_is_valid(int instance_id);

int instance_create(int owner_id, const char *name, enum e_instance_mode mode);
int instance_add_map(const char *name, int instance_id, bool usebasename, const char *map_name);
void instance_del_map(int m);
int instance_map2imap(int m, int instance_id);
int instance_mapname2imap(const char *map_name, int instance_id);
int16 instance_mapid(int16 m, int32 instance_id);
void instance_destroy(int instance_id);
enum e_instance_enter instance_enter(struct map_session_data *sd, int32 instance_id, const char *name, int16 x, int16 y);
void instance_init(int instance_id);

void instance_check_idle(int instance_id);
void instance_check_kick(struct map_session_data *sd);
void instance_set_timeout(int instance_id, unsigned int progress_timeout, unsigned int idle_timeout);
void instance_force_destroy(struct map_session_data *sd);

void do_reload_instance(void);
void do_final_instance(void);
void do_init_instance(void);

#endif
