# Database-Viewer-Editor (Old and sloppy code)
An application that allows you to view/edit a MySQL Database

Disclaimer: I was very new to object oriented programming 😅

## This application allows the user to do the following:
- Save database login information for ease of access, the login information is saved in a text file and read from it when connecting.
- Delete database login information
- List all databases saved and their status (Database label, IP and online or offline)
- Create/Delete tables from the database
- Read data from database
- Write data to database
- Delete data from database
- Basic music player


## Libraries used:
- SDL + SDL_Mixer (Music) https://www.libsdl.org/
- OTL (Database connection) http://otl.sourceforge.net/otl3_intro.htm

**NOTE**: Application requires MySQL Connector to be installed to connect to MySQL Databases
http://dev.mysql.com/downloads/connector/odbc/
