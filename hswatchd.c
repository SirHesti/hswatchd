/* *****************************************************************************
#-[ sed.file ]
hstex -x -r hswatchd.c -w sed.file     -e sed.file
s/$DATE/%DATE%/g
#-[ README.md
# hswatchd

## English

*I would like to point out that I always document my software in German.*

a daemon for monitoring files

hswatchd is designed for the lowest possible CPU performance. It is started via the *systemd*. If you want, feel free to create an init solution.

All parameters are entered in **_/etc/hswatchd.rc_**.

Each time a file has been changed, the function or file is executed.

## German / Deutsch

Ein Daemon zum Ueberwachen von Dateien

hswatchd ist ausgelegt auf kleinst moegliche CPU-Performance. Er wird ueber den *systemd* gestartet. Wer moechte kann sich gerne auch eine init-Loesung schreiben.

Alle Parameter werden in der **_/etc/hswatchd.rc_** eingetragen.

Jedesmal wenn eine Datei geaendert wurde, dann wird die eingestellte funktion, bzw. Datei ausgefuehrt.

#-[ hswatchd.man

---
title: hswatchd
section: 8
header: hswatchd - copyright Hesti
footer: hswatchd $VERSION
date: $DATE
---

# NAME
**hswatchd** - Ein Daemon zum Ueberwachen von Dateien

# SYNOPSIS
Daemonen werden ueber "systemd" gestartet. Dieser ueberwacht Dateien auf Aenderung und fuehrt, wenn vorhanden, einen Befehl aus.

# DESCRIPTION
| hswatchd ist ausgelegt auf kleinst moegliche CPU-Performance. Er wird ueber den *systemd* gestartet. Wer moechte kann sich gerne auch eine init-Loesung schreiben.
| Alle Parameter werden in der **_/etc/hswatchd.rc_** eingetragen.

# OPTIONS
**\-\-version **
:die aktuelle Version ausgeben lassen. Das Programm wird allerdings dann beendet.

Da hswatchd als Daemon gestartet wird, gibt es fuer den Betrieb keine Optionen.

# SIGNALS
**SIGHUP,SIGKILL,SIGINT**
: Der Daemon wird beendet

**SIGUSR1**
: reload der hswatchd.rc

**SIGUSR2**
: ausgabe der Infodatei

# ENVIRONMENT
Starten, stopppen, service:
: siehe Anschnitt: **SYSTEMD**

Konfiguration:
: siehe Anschnitt: **hswatchd.rc** 

# SYSTEMD

| Datei erstellen
| :/etc/systemd/system/HSwatchd.service

| Inhalt:

``` HSwatchd.service
[Unit]
Description=hswatchd - demon watching files for changing
After=network.target

[Service]
ExecStart=/usr/sbin/hswatchd
ExecStop=/bin/kill -HUP $MAINPID
ExecReload=/bin/kill -USR1 $MAINPID

StandardOutput=null
#StandardOutput=/root/out.txt
#Restart=on-failure

[Install]
WantedBy=multi-user.target
```

#EOF

| Service einrichten (einmalig, started danach automatisch mit):
``` Service
systemctl daemon-reload
systemctl enable HSwatchd.service
service HSwachtd start
```

# hswatchd.rc

| **Parameter:**
set time = \#\#\#
: Defaultangabe zum verweilen zwischen einer Pruefeinheit. Ist nicht so wichtig, da die Zeit automatisch errechnet wird.

set cache = _true_ oder _false_
: Die Benutzung des Caches erlauben/verbieten

set cachepath = _Pfad_zur_Cachedatei_
: Der Pfad für "hswatchd.cache". Es wird in dieser Datei die sekundengenaue Zeit der letzten Pruefung jedes file gespeichert. *Nur* der Pfad, die Datei heisst .hswatchd.cache.

set infofile = _File_fuer_InfoFile_
: Der vollständige Pfad zur Datei. Es wird eine reine Textdatei geschrieben. Ist Markdown formatierbar

Jeder Eintrag mit "file ...." in der hswatchd.rc wird in eine einfach verkettete Liste 
sortiert, nach naechster Bearbeitungszeit, aufgenommen. Dadurch wird erreicht das der 
naechste zu verarbeitende Datensatz ganz oben steht. Die Diff-Zeit wird auch genommen,
um die naechste Wartezeit zu setzen. Ist so ein Datensatz abgearbeitet, wird neu
entschieden, wo er in der Liste zu stehen hat. Und es wird eine neue Wartezeit
bestimmt.

| Konfigurations-Datei erstellen
| :/etc/hswatchd.rc

| Beispiel:

``` hswatchd.rc
# systemweite hswatchd - Configurationsdatei

set time = 5
set cache = true
set cachepath = /var/opt
set infofile = /pub/www/data/watch.txt

file /pub/share/network/stuff.txt       25  /bin/bash /root/bin/doany.sh inplement stuff
file /pub/share/network/admin_hosts     60  cp /pub/share/network/newfile_hosts /etc/hosts
```
#EOF
 
# AUTHORS
Heiko Stoevesandt - alias Hesti - <hstools@t-online.de>

# FEHLER
Fehler (auch in dieser Manpage) unbedingt bitte melden

#-[ hswatchd.rc ]
set time = 5
set cache = true
set cachepath = /var/cache
set infofile = /pub/www/data/watch.txt
#file /pub/share/network/smb.conf                            85 /bin/bash /pub/share/bin/netupd.sh smb
#file /pub/share/network/hosts                               60 /bin/bash /pub/share/bin/netupd.sh hosts
#-[ ende ----------- ]

 kill -s USR1 $(pidof hswatchd)
 wird die Config neu eingelesen (so aehnlich wie ein Reload)

 kill -s USR2 $(pidof hswatchd)
 schreib eine aktuelle Uebersicht in ein TextFile (Man beachte die Markdown
 Formatierung)

 Installation: siehe Manpage

  -------------------------------------------------------------------------------
 06.10.20 HS cache eingebaut
 28.10.22 HS infopath neu ueber usr2 aktuelles infos
 31.10.22 HS next in secs in der Uebrsicht
 01.11.22 HS Die einfach verkettete List LST wird permanent angepasst, so dass
             der naechste Job immer oben steht
 02.11.22 HS Manpage
 -------------------------------------------------------------------------------
*/

#include "tools.h"
#include "VERSION.h"
#include <signal.h>

void  *LST = NULL;
bool   useCache = false;
char  *CacheFile = NULL;
char  *InfoFile = NULL;
bool   bAbort = false;
int    loopState;
int    wait_time			__attribute__ ((unused)) ;
int    s_time;
time_t now;

enum mainLoop {
    LOOP_NORMAL,
    LOOP_STOP,
    LOOP_RELOAD,
	LOOP_INFOMODE
};
/**
 @brief blah

*/
typedef struct t_file {
    char *file;					///< file das zu prüfen ist
    char *execute;				///< Kommando, das ausgeführt werden soll
    time_t last_modif;			///< letzte modifizierung des Files
    time_t last_check;			///< wann zuletzt geprüft
    int interval;				///< wie oft zu prüfen
} t_file;

int ReadConfig(void);
int cfg_setcmd(char *Line);
int cfg_filecmd(char *Line);
int AddIDXByDate(struct n_Node *IDX);
int mainProcess(void);
int cleanup(void);
int UpdateFromCache(void);
int WriteOutCache(void);
int WriteInfo(void);
void m_sig (int sig);

signed int main(int argc, char *argv[])
{
    int rc;
    int print_msg;
    //printf ("%ld\n",sizeof(time_t)); return 27;

	if (InitTools(argc , argv, "%v%t%m", I_MAJOR, I_MINOR, I_BUILD, I_BETA, LOG_SYSLLOG)) return -1;
    lprintf("%s",m_PRG_INFO);
#ifdef OS_LINUX
    signal(SIGHUP,  m_sig);
    signal(SIGKILL, m_sig);
    signal(SIGINT,  m_sig);
    signal(SIGUSR1, m_sig);
    signal(SIGUSR2, m_sig);
#endif
    print_msg = 0;
    loopState = LOOP_RELOAD;
    for (;;)
    {
        if (loopState == LOOP_STOP) break;
        if (loopState == LOOP_INFOMODE)
		{
			WriteInfo();
			loopState = LOOP_NORMAL;
		}
        if (loopState == LOOP_RELOAD )
        {
            rc = cleanup();
            if (rc == EXIT_SUCCESS) if (ReadConfig()) rc = EXIT_FAILURE;
            if (rc == EXIT_SUCCESS) if (useCache==true) UpdateFromCache();
            if (rc != EXIT_SUCCESS) break;
            if (print_msg) lprintf("%s config reload complete",m_PRG_INFO);
            print_msg = 1;
			continue;
        }
		if (mainProcess()) // Tabelle abarbeiten
		{
			rc = EXIT_FAILURE;
			break;
		}
		if (wait_time>0)
        {
#ifdef OS_LINUX
            sleep(wait_time);
#else
            WriteInfo();
            break;
#endif
        }
    }
    cleanup();
    lprintf( "%s hat sich beendet", m_PRGNAME);
    return rc;
}

// kill -s KILL received
void m_sig(int sig)
{
	switch (sig)
	{
#ifdef OS_LINUX
		case SIGHUP:	// kill -s KILL received
    	case SIGKILL:
    	case SIGINT:
			loopState = LOOP_STOP;
			break;
    	case SIGUSR1:	// kill -s USR1 received
			loopState = LOOP_RELOAD;
			break;
    	case SIGUSR2:	// kill -s USR2 received
			loopState = LOOP_INFOMODE;
			break;
#endif // OS_LINUX
		default:
			break;
	}
}

//delete the complete Config
int cleanup(void)
{
    struct n_Node *my_IDX;
    t_file *DTA;

    for (;;)
    {
        if (LST==NULL) break;
        my_IDX = LST;
        LST = my_IDX->n_NEXT;
        if (my_IDX->n_DATA)
        {
            DTA = my_IDX->n_DATA;
            free (DTA->file);
            if (DTA->execute) free (DTA->execute);
            free (DTA);
        }
        free (my_IDX);
    }
    if (CacheFile) free(CacheFile);
    if (InfoFile) free(InfoFile);
    CacheFile = NULL;
    InfoFile = NULL;
    s_time   = 1;
    wait_time   = 1;
    loopState = LOOP_NORMAL;
    return EXIT_SUCCESS;
}

int cfg_filecmd(char *Line)
{
    char *a;
    int  t;
    struct stat st;
    struct n_Node *newNode;

    t_file *DTA;

    a = strdup(stridx(Line,1));                                                 // filename an Position 1
    t = atoi(stridx(Line,2));                                                   // Intervall an Position 2
    if ((t<1) || (t>86400) || (a[0]==0))
    {
        lprintf ("unknown (fail) cmd %s:", Line);
        free(a);
        return EXIT_FAILURE;
    }

    DTA = malloc0(sizeof(t_file));                                              // Datenrecord
    DTA->interval = t;
    DTA->file=a;
    DTA->last_check=0;
    if ((stat (DTA->file ,&st )==-1) || (!S_ISREG( st.st_mode )))               // Wenn File nicht gültig ist, dann ....
    {
        DTA->last_modif=0;
    }else{
        DTA->last_modif=st.st_mtime;
    }
    DTA->execute=NULL;
    for (t=3;;t++)
    {
        a = stridx(Line,t);
        if (a[0]==0) break;
        if (DTA->execute) stradd(DTA->execute," ");
        stradd(DTA->execute,a);
    }
    newNode = malloc (sizeof (struct n_Node));
    newNode->n_DATA=DTA;
	return AddIDXByDate(newNode);
    //LST = Node_Add(LST, DTA, false);
    //return EXIT_SUCCESS;
}

//# nicht t_file sondert mit IDX, aber Next wird überschieben
int AddIDXByDate(struct n_Node *IDX)
{
    struct n_Node *PREV;
    struct n_Node *my_IDX;
    t_file *my_DTA;
    t_file *DTA;
    DTA = IDX->n_DATA;

    PREV = NULL;
    for (my_IDX = LST;;)                                                        // sortiere Datensatz an der richtigen Position ein
    {
        if ( my_IDX == NULL ) break;                                            // nach dem letzen Datensatz ?
        my_DTA = my_IDX->n_DATA;                                                // Daten aus der Liste
        if ( (my_DTA->last_check + (time_t)my_DTA->interval) >                  // verglichen mit der aktuellen Zeit
             (   DTA->last_check + (time_t)   DTA->interval))
        {
            break;                                                              // dann an dieser Position
        }
        PREV = my_IDX;                                                          // den Datensatz als Vorgänger merken
        my_IDX = my_IDX->n_NEXT;                                                // Nächsten Datensatz hernehmen
    }
    //LST = Node_Add_After(LST, PREV, DTA);
    if (PREV == NULL)                                                           // Soll der erste Datensatz werden ?
    {
        IDX->n_NEXT = LST;
        LST = IDX;
        return EXIT_SUCCESS;
    }
    IDX->n_NEXT  = PREV->n_NEXT;                                                // Datensatz "einhaengen"
    PREV->n_NEXT = IDX;
    return EXIT_SUCCESS;
}

int cfg_setcmd(char *Line)
{
    char *a;
    a = stridx(Line,2);
    if (strcmp(a,"=")) return EXIT_FAILURE;

    if (!strcasecmp(stridx(Line,1),"time"))
    {
        int s;
        s = atoi(stridx(Line,3));
        if ( (s<1) || (s>86400) )
        {
            lprintf ("time range must set >0 <86400");
            return EXIT_FAILURE;
        }
        s_time = s;
        wait_time = s;
        return EXIT_SUCCESS;
    }
    if (!strcasecmp(stridx(Line,1),"cache"))
    {
        useCache = GetIntTrueFalsefromString(stridx(Line,3));
        return EXIT_SUCCESS;
    }

    if (!strcasecmp(stridx(Line,1),"cachepath"))
    {
        if (CacheFile) free (CacheFile);
        CacheFile = strprintf ("%s/.%s.cache",stridx(Line,3), m_PRGNAME);
        return EXIT_SUCCESS;
    }
    if (!strcasecmp(stridx(Line,1),"infofile"))
    {
        if (InfoFile) free (InfoFile);
        InfoFile = strdup(stridx(Line,3));
        return EXIT_SUCCESS;
    }

    lprintf ("unknown set cmd %s:", Line);
    return EXIT_FAILURE;
}

int ReadConfig(void)
{
    int rc;
    FILE *cfg;
    char cfg_Filename[PATH_MAX];
    char Line[PATH_MAX*2];
    char *arg;

#ifdef OS_LINUX
    sprintf_ex (cfg_Filename,"/etc/%s.rc", m_PRGNAME);
    strlwr  (cfg_Filename);
#else
    sprintf_ex (cfg_Filename,"C:/hs/etc/%s", m_PRGNAME);
#endif // OS_LINUX

    if(( cfg = fopen(cfg_Filename,"rt")) == NULL )
    {
        lprintf ("Config not found: %s", cfg_Filename);
        return EXIT_FAILURE;
    }
	rc = EXIT_SUCCESS;
    while (rc == EXIT_SUCCESS)
    {
        if (mgetline(cfg,Line,sizeof(Line)-3)!=0) break;                        // Zeile einlesen
        CL(Line);                                                               // bereinigen
        if (Line[0]==0) continue;
        if (Line[0]=='#') continue;                                             // Kommentarzeilen

        arg = stridx(Line,0);                                                   // Erstes Argument -> Kommandoverteilung
             if (!strcasecmp(arg,"set"))  rc = cfg_setcmd(Line);
        else if (!strcasecmp(arg,"file")) rc = cfg_filecmd(Line);
        else
        {
            lprintf ("unknown CMD: %s", Line);
            rc = EXIT_FAILURE;
        }
    }
    fclose (cfg);
    if (useCache) if (!CacheFile) useCache=false;
    //if (useCache) lprintf ("Cache: %s", CacheFile);
    return rc;
}

// Hier wird geprüft ob eine Datei geprüft werden soll
// 1. prüfung ob der letzte check schon $interval her ist
// 2. wenn, dann stat auf das file.
//    wenn fehlgesclagen, dann ende
// 3. wurde das File modifiziert, nein dann ende
// 4. Execute
//-----------------------------------------------------------------------------
int mainProcess(void)
{
    void    *LNXT;
    t_file  *DTA;
    struct   stat st;
    time_t   diff;

    now = unixtime();
    if (!LST)  return EXIT_SUCCESS;
    LNXT = LST;
    DTA = Node_GetData(LNXT);
    diff = now - (DTA->last_check + DTA->interval);                             // solange "check+interval < now" nicht bearbeiten
    if (diff < 0)
    {
        diff = -diff;
        wait_time =(int)diff;
        if (wait_time < s_time) wait_time = s_time;
        return EXIT_SUCCESS;
    }
    DTA->last_check = now;			    								        // letzte pruefzeit(check) uebertragen
    if ((stat (DTA->file ,&st )==-1) || (!S_ISREG( st.st_mode )))               // Wenn File nicht gültig, incl. das das File gelöscht wurde
    {
        DTA->last_modif=0;												        // modif auf 1.1.1970 setzen
    }else{
        if (DTA->last_modif != st.st_mtime)         							// gemerkte mtime(modif) geandert, ja?
        {
            DTA->last_modif = st.st_mtime;										// mtime(modif) geandert merken
            lprintf ("Change modif detect: %s ==> execute : %s", DTA->file, DTA->execute);  // file changed detect
            if (system (DTA->execute)) lprintf ("system failure %s", DTA->execute);	// kann auch schiefgehem
            if (useCache) WriteOutCache();										// cache noch schreiben
        }
    }
    //LNXT isolieren, dass ist der erste Datensatz in LST
    LST = Node_GetNext(LNXT);
    wait_time = 0;
    return AddIDXByDate(LNXT);
}
#ifdef OLDIE
int mainProcess(void)
{
    void    *LNXT;
    t_file  *DTA;
    struct stat st;

    now = unixtime();
    for (LNXT = LST;;LNXT=Node_GetNext(LNXT))									// Durch die gesamte Liste gehen
    {
        if (!LNXT)  return EXIT_SUCCESS;										// Ende der Liste?
        DTA = Node_GetData(LNXT);												// Datenpaket Zeiger setzen
																				// lprintf ("check f=%s m=%ld l=%ld", DTA->file, DTA->last_modif, DTA->last_check);
        if (now < (DTA->last_check + DTA->interval) ) continue;					// solange "check+interval < now" nicht bearbeiten
        DTA->last_check = now;													// letzte pruefzeit(check) uebertragen
        if ((stat (DTA->file ,&st )==-1) || (!S_ISREG( st.st_mode )))           // Wenn File nicht gültig, incl. das das File gelöscht wurde
        {
            DTA->last_modif=0;												    // modif auf 1.1.1970 setzen
            continue;
        }
        if (DTA->last_modif == st.st_mtime) continue;							// gemerkte mtime(modif) geandert, nein, dann naechster
        DTA->last_modif = st.st_mtime;											// mtime(modif) geandert merken
        lprintf ("Change modif detect: %s ==> execute : %s", DTA->file, DTA->execute);  // file changed detect
        if (system (DTA->execute)) lprintf ("system failure %s", DTA->execute);	// kann auch schiefgehem
        if (useCache) WriteOutCache();											// cache noch schreiben
    }
    return EXIT_SUCCESS;
}
#endif




// Cache einlesen. dh last_modif wird gesetzt
//-----------------------------------------------------------------------------
int UpdateFromCache(void)
{
    char *tr;
    char Line[PATH_MAX+32];
    void    *LNXT;
    t_file  *DTA;
    FILE *cf;

    //lprintf ("Load Cache");
    if(( cf = fopen(CacheFile,"rt")) == NULL )
    {
        lprintf ("can't load Cache; if this the firstrun OK; try to create");
        return WriteOutCache();
    }
    //**??modif auf null setzten??**//
    for (;;)
    {
        if (mgetline(cf,Line,sizeof(Line)-3)!=0) break;
        tr = strchr(Line,',');
        if (!tr) continue;

        tr[0]='\0';
        tr++;

        for (LNXT = LST;;LNXT=Node_GetNext(LNXT))
        {
            if (!LNXT) break;;
            DTA = Node_GetData(LNXT);
            if (!strcmp(tr,DTA->file))
            {
                DTA->last_modif=0;
                for (tr = Line;;tr++)
                {
                    if (!isdigit(tr[0])) break;
                    DTA->last_modif = (DTA->last_modif * 10) + (tr[0]-'0') ;
                }
                break;
            }
        }
    }
    fclose(cf);
    return EXIT_SUCCESS;
}

//Cache schreiben
int WriteOutCache(void)
{
    void    *LNXT;
    t_file  *DTA;
    FILE *cf;
    //lprintf ("try Cache wrote: %s", CacheFile);
	if (!CacheFile) return EXIT_FAILURE;
    if(( cf = fopen(CacheFile,"wt")) == NULL )
    {
        lprintf ("Cache not ready to write: %s", CacheFile);
        return EXIT_FAILURE;
    }

    for (LNXT = LST;;LNXT=Node_GetNext(LNXT))
    {
        if (!LNXT) break;
        DTA = Node_GetData(LNXT);
        fprintf (cf, TIME_STR_LD ",%s\n",DTA->last_modif, DTA->file);
    }
    fclose (cf);
    //lprintf ("Cache wrote");
    return EXIT_SUCCESS;
}

int WriteInfo(void)
{
    void    *LNXT;
    t_file  *DTA;
    FILE *cf;
	int i;
	int sz_file;
	int len_file;

	char da_modif[32];
	char da_check[32];

    if(!InfoFile)
    {
        lprintf ("InfoFile (NULL) not set");
        return EXIT_FAILURE;
    }

    if(( cf = fopen(InfoFile,"wt")) == NULL )
    {
        lprintf ("InfoFile not ready to write: %s", InfoFile);
        return EXIT_FAILURE;
    }

	sz_file = 26;
	for (LNXT = LST;;LNXT=Node_GetNext(LNXT))
    {
		if (!LNXT) break;
		DTA = Node_GetData(LNXT);
		len_file = strlen(DTA->file);
		if (len_file>=sz_file) sz_file=len_file+1;
	}

	//if (HS_DEBUG_TRUE)
	fprintf (cf, "s=%i w=%i ->", s_time, wait_time);
	fprintf (cf, "LastCheck: %s\n", strtime(now,2));
	fprintf (cf, "%-*s | sec | Date:Modif          | Date:checked        | next | Execute\n", sz_file, "File");
	for (i=0;i<sz_file;i++)
	    fprintf (cf, "-");
	fprintf (cf, "-|-----|---------------------|---------------------|------|--------\n");
	for (LNXT = LST;;LNXT=Node_GetNext(LNXT))
    {
        if (!LNXT) break;
        DTA = Node_GetData(LNXT);
        //fprintf (cf, TIME_STR_LD ",%s\n",DTA->last_modif, DTA->file);
		strcpy (da_modif, strtime(DTA->last_modif,2));
		strcpy (da_check, strtime(DTA->last_check,2));
		fprintf (cf, "%-*s |%4i | %s | %s |%5i | %s\n", sz_file, DTA->file, DTA->interval, da_modif, da_check, (int)(DTA->last_check+((time_t)DTA->interval)-now),  DTA->execute);
    }
    fclose (cf);
	lprintf ("InfoFile ready: %s", InfoFile);
    return EXIT_SUCCESS;
}
