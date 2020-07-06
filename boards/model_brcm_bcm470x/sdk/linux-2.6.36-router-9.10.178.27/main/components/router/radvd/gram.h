#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

#ifndef YYSTYPE
typedef union {
	unsigned int		num;
	int			snum;
	double			dec;
	int			bool;
	struct in6_addr		*addr;
	char			*str;
	struct AdvPrefix	*pinfo;
	struct AdvRoute		*rinfo;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	T_INTERFACE	257
# define	T_PREFIX	258
# define	T_ROUTE	259
# define	STRING	260
# define	NUMBER	261
# define	SIGNEDNUMBER	262
# define	DECIMAL	263
# define	SWITCH	264
# define	IPV6ADDR	265
# define	INFINITY	266
# define	T_IgnoreIfMissing	267
# define	T_AdvSendAdvert	268
# define	T_MaxRtrAdvInterval	269
# define	T_MinRtrAdvInterval	270
# define	T_MinDelayBetweenRAs	271
# define	T_AdvManagedFlag	272
# define	T_AdvOtherConfigFlag	273
# define	T_AdvLinkMTU	274
# define	T_AdvReachableTime	275
# define	T_AdvRetransTimer	276
# define	T_AdvCurHopLimit	277
# define	T_AdvDefaultLifetime	278
# define	T_AdvDefaultPreference	279
# define	T_AdvSourceLLAddress	280
# define	T_AdvOnLink	281
# define	T_AdvAutonomous	282
# define	T_AdvValidLifetime	283
# define	T_AdvPreferredLifetime	284
# define	T_AdvRouterAddr	285
# define	T_AdvHomeAgentFlag	286
# define	T_AdvIntervalOpt	287
# define	T_AdvHomeAgentInfo	288
# define	T_Base6to4Interface	289
# define	T_UnicastOnly	290
# define	T_HomeAgentPreference	291
# define	T_HomeAgentLifetime	292
# define	T_AdvRoutePreference	293
# define	T_AdvRouteLifetime	294
# define	T_AdvMobRtrSupportFlag	295
# define	T_BAD_TOKEN	296


extern YYSTYPE yylval;

#endif /* not BISON_Y_TAB_H */
