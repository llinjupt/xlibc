#ifndef XFCFF_H
#define XFCFG_H

/* lli_njupt@163.com created 2013, updated at 2016/12/03 */

#define XFCFG_NAME_MAX_LEN    (64)
#define XFCFG_DEFAULT_MAX_LEN (256)

typedef struct
{
	char name[XFCFG_NAME_MAX_LEN];

  /* parse func */	
	int (*parse)(const char *line, void *var);
	void *value;

	char def[XFCFG_DEFAULT_MAX_LEN]; /* default value */
}xfcfg_option_t;

int xfcfg_parse_bool(const char *value, void *arg);
int xfcfg_parse_int(const char *value, void *arg);
int xfcfg_parse_str(const char *value, void *arg);
int xfcfg_parse_ipv4(const char *value, void *arg);
int xfcfg_parse_ipv4str(const char *value, void *arg);
int xfcfg_parse_mac(const char *value, void *arg);
int xfcfg_parse_size(const char *value, void *arg);

#endif /* XFCFG_H */
