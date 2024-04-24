# hswatchd

## English
*I would like to point out that I always document my software in German.*
a daemon for monitoring files
hswatchd is designed for the lowest possible CPU performance. It is started via the *systemd*. If you want, feel free to create an init solution.
All parameters are entered in **_/etc/hswatchd.rc_**. An example is included or can be copied from [github](https://github.com/SirHesti/hswatchd).
Each time a file has been changed, the function or file is executed.

## German / Deutsch
Ein Daemon zum Ueberwachen von Dateien
hswatchd ist ausgelegt auf kleinst moegliche CPU-Performance. Er wird ueber den *systemd* gestartet. Wer moechte kann sich gerne auch eine init-Loesung schreiben.
Alle Parameter werden in der **_/etc/hswatchd.rc_** eingetragen. Eine Beispiel wird mitgeliefert oder kann auf von [github](https://github.com/SirHesti/hswatchd) kopiert werden.
Jedesmal wenn eine Datei geaendert wurde, dann wird die eingestellte Funktion, bzw. Datei ausgefuehrt.


---
title: hswatchd
section: 8
header: hswatchd - copyright Hesti
footer: hswatchd 2.12.5 Beta
date: 24.04.2024
---

### NAME
**hswatchd** - Ein Daemon zum Ueberwachen von Dateien

### SYNOPSIS
Daemonen werden ueber "systemd" gestartet. Dieser ueberwacht Dateien auf Aenderung und fuehrt, wenn vorhanden, einen Befehl aus.

### DESCRIPTION
| hswatchd ist ausgelegt auf kleinst moegliche CPU-Performance. Er wird ueber den *systemd* gestartet. Wer moechte kann sich gerne auch eine init-Loesung schreiben.
| Alle Parameter werden in der **_/etc/hswatchd.rc_** eingetragen.

### OPTIONS
**\-\-version **
:die aktuelle Version ausgeben lassen. Das Programm wird allerdings dann beendet.

Da hswatchd als Daemon gestartet wird, gibt es fuer den Betrieb keine Optionen. Dafür wird eine hswatchd.rc erwartet.

### SIGNALS
**SIGHUP,SIGKILL,SIGINT**
: Der Daemon wird beendet

**SIGUSR1**
: reload der hswatchd.rc

**SIGUSR2**
: ausgabe der Infodatei

### ENVIRONMENT
Starten, stopppen, service:
: siehe Anschnitt: **SYSTEMD**

Konfiguration:
: siehe Anschnitt: **hswatchd.rc**

### SYSTEMD
Sicher gibt es viele Wege um einen Daemon zu laufen zu bekommen. Derzeit wird aber SYSTEMD ueberall verwendet.

Achtung. Zum grossen Teil werden ROOT-Rechte noetig sein.

| Datei erstellen
| :/etc/systemd/system/hswatchd.service

| Inhalt:

``` hswatchd.service
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
systemctl enable hswatchd.service
systemctl start hswatchd.service
systemctl status hswatchd
```

### hswatchd.rc

| **Parameter:**

\### Server Einstellungen
: Zeilen, die mit '#' beginnen, sind Kommentare und werden entsprechend ignoriert. Uebrigens auch Leerzeilen

port        = 8080
: Portangabe. hswatchd kann über http abgefragt werden
: siehe Anschnitt: **HTTP*

time        = \#\#\#
: Defaultangabe zum verweilen zwischen einer Pruefeinheit. Ist nicht so wichtig, da die Zeit automatisch errechnet wird.

cache       = _true_ oder _false_
: Die Benutzung des Caches erlauben/verbieten

cachepath   = _Pfad_zur_Cachedatei_
: Der Pfad für eine chache-Datei. Es wird in dieser Datei die sekundengenaue Zeit der letzten Pruefung jedes File gespeichert. Gefolgt von dem Dateinamen.

infoupdate  = 3600
: In diesem Fall wird alle 3600 Sekunden ein Infofile erstellt.

infofile    = _File_fuer_InfoFile_
: Der vollständige Pfad zur Datei. Es wird eine reine Textdatei geschrieben. Ist Markdown formatierbar. Diese Angabe kann auch leer sein (oder besser
auskommentiert) werden.

Jeder Eintrag mit "file ...." in der hswatchd.rc wird in eine einfach verkettete Liste
sortiert, nach naechster Bearbeitungszeit, aufgenommen. Dadurch wird erreicht das der
naechste zu verarbeitende Datensatz ganz oben steht. Die Diff-Zeit wird auch genommen,
um die naechste Wartezeit zu setzen. Ist so ein Datensatz abgearbeitet, wird neu
entschieden, wo er in der Liste zu stehen hat. Und es wird eine neue Wartezeit
bestimmt. Diese Informationen koennen auch über http abgerufen werden !!

| Konfigurations-Datei erstellen
| :/etc/hswatchd.rc

##### Beispiel:

``` hswatchd.rc
### systemweite hswatchd - Konfigurationsdatei

port        = 8080
time        = 1
cache       = true
cachepath   = /var/hswatchd/files.cache
infoupdate  = 3600
infofile    = /srv/pub/www/data/watch-debug-hesti.txt

### File - Section
file /pub/share/network/stuff.txt       25  /bin/bash /root/bin/doany.sh inplement stuff
file /pub/share/network/admin_hosts     60  cp /pub/share/network/newfile_hosts /etc/hosts
```

### HTTP-Service
hswatchd verfügt eine rudimentäre html-Schnittstelle. Hier lassen sich ein paar Informationen abrufen.
Der Port ist in der _hswatchd.rc_ zu definiert. 

_server:8080/reload.cgi_
: wird hswatchd dazu veranlassen die hswatchd.rc neu zu laden.

_server:8080//date.cgi_
: gibt das aktuelle datum mit der Zeit aus. Das war nur zum Testen drin, aber es stoert ja auch niemanden.

_server:8080//nextwatch.html_
: Eine ausfuerliche Seite, welche Dateien als naechstes geprueft werden und eine kurze Uebersicht welche Parameter mit wirken.

_server:8080//drive.cgi?_
: Gefolgt von dem Mount-Point gibt ein paar Informationen ueber gemountete Laufwerke auf dem Server preis. Diese koennen so
gezielt gesammelt werden. Ein Aufruf wie _server:8080//drive.cgi?&boot_ wird die RootPartition und die boot-Partition ausgeben.

#EOF

### AUTHORS
Heiko Stoevesandt - alias Hesti - <hstools@t-online.de>

### FEHLER
Fehler (auch in dieser Manpage) unbedingt bitte melden

