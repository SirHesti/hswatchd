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


---
title: hswatchd
section: 8
header: hswatchd - copyright Hesti
footer: hswatchd 2.12.5 Beta
date: 21.04.2024
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

Da hswatchd als Daemon gestartet wird, gibt es fuer den Betrieb keine Optionen.

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
### systemweite hswatchd - Configurationsdatei

set time = 5
set cache = true
set cachepath = /var/opt
set infofile = /pub/www/data/watch.txt

file /pub/share/network/stuff.txt       25  /bin/bash /root/bin/doany.sh inplement stuff
file /pub/share/network/admin_hosts     60  cp /pub/share/network/newfile_hosts /etc/hosts
```
#EOF

### AUTHORS
Heiko Stoevesandt - alias Hesti - <hstools@t-online.de>

### FEHLER
Fehler (auch in dieser Manpage) unbedingt bitte melden

