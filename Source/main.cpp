#include <iostream>
#include <vector>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <conio.h>
#define OTL_STL
#define OTL_ODBC_MYSQL
#define OTL_ODBC // Compile OTL 4.0/ODBC
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE
// The following #define is required with MyODBC 3.51.11 and higher
#include "otlv4.h" // include the OTL 4.0 header file
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>


otl_connect db;
std::ifstream fstream_IN;
std::ofstream fstream_OUT;

Mix_Music* g_music;

std::vector<std::string> v_DatabaseLabels;

std::string connectionString;
const std::string defaultPath = "defaultmusic.mp3";
std::string filePath;
std::string oldPath;
char path;

enum TPathType {

    PATH_NOT_SET = -1,
	PATH_FAILURE = 0,
	PATH_SUCCESS = 1

};




short g_defaultVolume;
//line = "---------------------------------------------------------";

struct Connection
{
	std::string label = "DefaultDatabaseLabel"; //Default value = DefaultDatabaseLabel
	std::string driver = "MySQL ODBC 5.3 ANSI Driver"; //Default value = MySQL ODBC 5.3 ANSI Driver
	std::string server = "localhost"; //Default value = localhost
	std::string port = "3306"; //Default value = 3306
	std::string database = "test"; //Default value = test
	std::string user = "root"; //Default value = root
	std::string pass;

}defaultCONNECTION, currentCONNECTION;


void saveDatabaseLabel();
void deleteDatabaseLabel();
void listDatabaseStatus();

bool connectToDatabase(std::string &p_label);

void mainMenu();
void musicSettings();

void tableMenu();
void readDataMenu();
void writeDataMenu();
void deleteDataMenu();

void showDatabaseLabels();

void createTable(std::string &p_table, std::string &p_attributes);
void deleteTable(std::string &p_table);

void selectAll(std::string &p_table);
void selectIndex(std::string &p_select, std::string &p_table);
void selectCustom(std::string &p_SQL);

void insertAll(std::string &p_table, std::string &p_values);
void insertIndex(std::string &p_table,std::string &p_columns, std::string &p_values);
void insertCustom(std::string &p_SQL);

void deleteIndex(std::string &p_table, std::string &p_columns, std::string &p_values);
void deleteCustom(std::string &p_SQL);

void stringLoop(std::string string, int milliseconds);

void stopMusic(Mix_Music* p_music);
bool startMusic(Mix_Music* p_music, std::string p_musicfile);


int main(int argc, char** argv)
{
	
	//Custom variable type to check if path is set or not for music file
	path = TPathType::PATH_NOT_SET;


	filePath = defaultPath;
	g_defaultVolume = 10;

	//Initialize audio
	SDL_Init(SDL_INIT_AUDIO);

	//Custom function, play selected music, startMusic(music object, path for music file)
	startMusic(g_music, defaultPath.c_str());

	//Console window size
	COORD coord;
	coord.X = 81;
	coord.Y = 20000;

	//Adjust buffer size
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!SetConsoleScreenBufferSize(handle, coord))
		throw std::runtime_error("Unable to resize screen buffer.");
	
	//Initialize OTL (database library)
	otl_connect::otl_initialize();


	//Custom function
	mainMenu();

	//Custom function, stop selected music object
	stopMusic(g_music);
}


void saveDatabaseLabel()
{
	std::string choice;
	std::string line;

	bool bSaved = false;
	bool bNameExists = false;


	//Open alldatabases.txt and put all the labels in a vector
	fstream_IN.open("alldatabases.txt");
	while (getline(fstream_IN, line))
	{
		v_DatabaseLabels.push_back(line);
	}
	fstream_IN.close();

	//Create database label
	do
	{
		std::cout << "---------------------------------CREATING LABEL---------------------------------" << std::endl << std::endl;
		std::cout << "Tip: Leave the field empty for default value (Press ENTER for default value)" << std::endl;
		std::cout << "--------------------------------------------------------------------------------" << std::endl;

		//Ask for a label name until the label name doesn't exist
		do{
			std::cout << "Enter a label for your database (Default: DefaultDatabaseLabel): ";
			getline(std::cin, currentCONNECTION.label);

			if (currentCONNECTION.label == "")
			{
				currentCONNECTION.label = defaultCONNECTION.label;
			}


			if (v_DatabaseLabels.size() > 0)
			{
				for (int i = 0; i < v_DatabaseLabels.size(); i++)
				{
					if (v_DatabaseLabels.at(i) == currentCONNECTION.label)
					{
						bNameExists = true;
						std::cout << "The name " << currentCONNECTION.label << " already exists! Try another name..." << std::endl << std::endl;

					}
					else
					{
						bNameExists = false;
					}

				}

			}

		} while (bNameExists);


		std::cout << "Enter the driver of your database (Default: MySQL ODBC 5.3 ANSI Driver): ";
		getline(std::cin, currentCONNECTION.driver);
		std::cout << "Enter the IP (Default: localhost): ";
		getline(std::cin, currentCONNECTION.server);
		std::cout << "Enter the port (Default: 3306): ";
		getline(std::cin, currentCONNECTION.port);
		std::cout << "Enter the database name (Default: test): ";
		getline(std::cin, currentCONNECTION.database);
		std::cout << "Enter the username (Default: root): ";
		getline(std::cin, currentCONNECTION.user);
		std::cout << "Enter the password: ";
		getline(std::cin, currentCONNECTION.pass);
		std::cout << "----------------------------------------------------------------------------" << std::endl;

		v_DatabaseLabels.clear();


		if (currentCONNECTION.driver == "")
		{
			currentCONNECTION.driver = defaultCONNECTION.driver;
		}
		if (currentCONNECTION.server == "")
		{
			currentCONNECTION.server = defaultCONNECTION.server;
		}
		if (currentCONNECTION.port == "")
		{
			currentCONNECTION.port = defaultCONNECTION.port;
		}
		if (currentCONNECTION.database == "")
		{
			currentCONNECTION.database = defaultCONNECTION.database;
		}
		if (currentCONNECTION.user == "")
		{
			currentCONNECTION.user = defaultCONNECTION.user;
		}


		connectionString = "DRIVER={" + currentCONNECTION.driver + "};SERVER=" + currentCONNECTION.server + ";PORT=" + currentCONNECTION.port + ";DATABASE=" + currentCONNECTION.database + ";USER=" + currentCONNECTION.user + ";PASSWORD=" + currentCONNECTION.pass + ";";

		//Loop through the connection string to make a small effect
		for (int i = 0; i < connectionString.length(); i++)
		{
			std::cout << connectionString.at(i);
			Sleep(30);

			//Everytime the current index of the string is ; start writing on next line
			if (connectionString.at(i) == ';')
			{
				std::cout << "\n";
			}
		}
            std::cout << std::endl << "Save? (Y or N) type 'cancel' to return to main menu: ";
        do
        {

            getline(std::cin, choice);

            if (choice == "N" || choice == "n")
            {
                bSaved = false;
                system("cls");
            }
            else if (choice == "Y" || choice == "y")
            {
                bSaved = true;
                system("cls");
            }
			else if (choice == "cancel")
			{

			}
            else
            {
                std::cout << std::endl << "Enter Y or N..." << std::endl;
            }
        }while(choice != "Y" && choice != "y" && choice != "N" && choice != "n");

	} while (!bSaved);



	//

	fstream_OUT.open("databaselogin.txt", std::ios::app);
	fstream_OUT << currentCONNECTION.label << std::endl << currentCONNECTION.driver << std::endl << currentCONNECTION.server << std::endl << currentCONNECTION.port << std::endl << currentCONNECTION.database << std::endl << currentCONNECTION.user << std::endl << currentCONNECTION.pass << std::endl << "---------------------------------------------------------" << std::endl;
	fstream_OUT.close();

	fstream_OUT.open("alldatabases.txt", std::ios::app);
	fstream_OUT << currentCONNECTION.label << std::endl;
	fstream_OUT.close();


	mainMenu();

}

void deleteDatabaseLabel()
{

    std::string line;

    fstream_IN.open("alldatabases.txt");
        while(getline(fstream_IN, line))
        {
            v_DatabaseLabels.push_back(line);
        }
    fstream_IN.close();


    if(v_DatabaseLabels.size() > 0)
    {

        std::string choice;

        std::ofstream temp;

        bool bNameExists = false;


        std::cout << "---------------------------------DELETING LABEL---------------------------------" << std::endl << std::endl;
        std::cout << "Enter the name of the label you wish to delete" << std::endl;
        std::cout << "Type cancel to return to main menu" << std::endl;
        std::cout << "--------------------------------------------------------------------------------" << std::endl;
        fstream_IN.open("alldatabases.txt");
        while(getline(fstream_IN, line))
        {
            std::cout << line << std::endl;
        }
        fstream_IN.close();
        std::cout << "--------------------------------------------------------------------------------" << std::endl;

        do{
           getline(std::cin, choice);


            if(choice == "cancel")
            {
                system("cls");
                v_DatabaseLabels.clear();
				mainMenu();
            }


            if(v_DatabaseLabels.size() > 0)
            {
              for(int i = 0; i < v_DatabaseLabels.size();i++)
              {
                  if(choice != v_DatabaseLabels.at(i))
                  {
                      bNameExists = false;
                  }
                  else
                  {
                      bNameExists = true;
                      break;
                  }
              }
            }

            if(!bNameExists)
            {
              std::cout << std::endl << "The database " << choice << " does not exist! Try again..." << std::endl << std::endl;

            }


        }while(!bNameExists);




        fstream_IN.open("databaselogin.txt");
        temp.open("temp.txt");

        while(getline(fstream_IN, line))
        {
            if(line != choice)
            {
                temp << line << std::endl;
            }
            else if(line == choice)
            {
                getline(fstream_IN, line);
                getline(fstream_IN, line);
                getline(fstream_IN, line);
                getline(fstream_IN, line);
                getline(fstream_IN, line);
                getline(fstream_IN, line);
                getline(fstream_IN, line);
            }

        }


        fstream_IN.close();
        temp.close();


          if( remove( "databaselogin.txt" ) != 0 )
            perror( "Could not edit file!" );

        rename("temp.txt", "databaselogin.txt");

        fstream_IN.open("alldatabases.txt");
        temp.open("temp.txt");

        while(getline(fstream_IN, line))
        {
            if(line != choice)
            {
               temp << line << std::endl;
            }
        }

        fstream_IN.close();
        temp.close();

      if( remove( "alldatabases.txt" ) != 0 )
        perror( "Could not edit file/remove database!\n" );
      else
      {
         std::cout << "Database " << choice << " successfully removed!" << std::endl << std::endl;
      }



        rename("temp.txt", "alldatabases.txt");




        system("PAUSE");
        system("cls");


    }
    else
    {
        std::cout << "Theres no databases saved! Go create one." << std::endl;
        system("PAUSE");
        system("cls");
    }

    v_DatabaseLabels.clear();
	mainMenu();

}

void listDatabaseStatus()
{
    std::string line;
    Connection* tempConn = new Connection;

	fstream_IN.open("alldatabases.txt");
	while (getline(fstream_IN, line))
	{
		v_DatabaseLabels.push_back(line);
	}
	fstream_IN.close();




    if(v_DatabaseLabels.size() > 0)
    {


	std::string tempConnectionString;
	std::string tempString;


        std::cout << "-----------------------------------DATABASES-----------------------------------" << std::endl << std::endl;;
        std::cout << "LABEL                              IP-ADRESS                            STATUS" << std::endl;
        std::cout << "===============================================================================" << std::endl;
        fstream_IN.open("databaselogin.txt");
        while (getline(fstream_IN, line))
        {
            if (v_DatabaseLabels.size() > 0)
            {

                for (int i = 0; i < v_DatabaseLabels.size(); i++)
                {


                    if (v_DatabaseLabels.at(i) == line)
                    {

                        getline(fstream_IN, tempConn->driver);
                        getline(fstream_IN, tempConn->server);
                        getline(fstream_IN, tempConn->port);
                        getline(fstream_IN, tempConn->database);
                        getline(fstream_IN, tempConn->user);
                        getline(fstream_IN, tempConn->pass);


                        tempConnectionString = "DRIVER={" + tempConn->driver + "};SERVER=" + tempConn->server + ";PORT=" + tempConn->port + ";DATABASE=" + tempConn->database + ";USER=" + tempConn->user + ";PASSWORD=" + tempConn->pass + ";";
                        try
                        {


                            std::cout << v_DatabaseLabels.at(i);

                            tempString = v_DatabaseLabels.at(i);
                            for (int i = 0; i < 35 - tempString.length(); i++)
                            {
                                std::cout << " ";
                            }

                            std::cout << tempConn->server;

                            for (int i = 0; i < 37 - tempConn->server.length(); i++)
                            {
                                std::cout << " ";
                            }

                            db.rlogon(tempConnectionString.c_str());
                            std::cout << "ONLINE" << std::endl;
                        }
                        catch (otl_exception &e)
                        {
                            std::cout << "OFFLINE" << std::endl;
                        }


                        db.logoff();

                    }
                }
            }


        }
    }
    else
    {
        std::cout << "-----------------------------------DATABASES-----------------------------------" << std::endl << std::endl;;
        std::cout << "LABEL                              IP-ADRESS                            STATUS" << std::endl;
        std::cout << "===============================================================================" << std::endl;
        std::cout << "No databases found! Go create one." << std::endl;
    }



	fstream_IN.close();

	std::cout << std::endl;
	if(v_DatabaseLabels.size() > 0)
    {
        system("PAUSE");
    }
    else
    {
        Sleep(2000);
    }
    delete tempConn;
	v_DatabaseLabels.clear();
	system("cls");
	mainMenu();
}

bool connectToDatabase(std::string &p_label)
{

	std::string line;
	std::string tempConnectionString;

	bool bNameExists = false;



	Connection* tempConn = new Connection;

	fstream_IN.open("databaselogin.txt");
	while (getline(fstream_IN, line))
	{
		if (line == p_label)
		{
			getline(fstream_IN, tempConn->driver);
			getline(fstream_IN, tempConn->server);
			getline(fstream_IN, tempConn->port);
			getline(fstream_IN, tempConn->database);
			getline(fstream_IN, tempConn->user);
			getline(fstream_IN, tempConn->pass);
		}
	}
	fstream_IN.close();


	tempConnectionString = "DRIVER={" + tempConn->driver + "};SERVER=" + tempConn->server + ";PORT=" + tempConn->port + ";DATABASE=" + tempConn->database + ";USER=" + tempConn->user + ";PASSWORD=" + tempConn->pass + ";";
	delete tempConn;
	try
	{
		db.rlogon(tempConnectionString.c_str());
		return true;

	}
	catch (otl_exception &e)
	{

		v_DatabaseLabels.clear();
		system("cls");
		return false;
	}
}

void mainMenu()
{

	std::string choice;

	std::cout << "====================================WELCOME=====================================" << std::endl << std::endl;
	std::cout << "Enter the number of the option you wish to proceed with" << std::endl;
	std::cout << "NOTE: MySQL experience might be required!" << std::endl;
	std::cout << "----------------------------------MAIN OPTIONS----------------------------------" << std::endl;
	std::cout << "1. Save database login information" << std::endl;
	std::cout << "2. Delete database login information" << std::endl;
	std::cout << "3. List database status" << std::endl;
	std::cout << "----------------------------------EDIT OPTIONS----------------------------------" << std::endl;
	std::cout << "4. Create/Delete table                              6. Write data to database" << std::endl;
	std::cout << "5. Read data from database                          7. Delete data from database" << std::endl;
	std::cout << "-------------------------------------OTHER--------------------------------------" << std::endl;
	std::cout << "P. Pause/Play music                                 S. Music Settings" << std::endl;

	std::cout << "Q. Quit" << std::endl;
	std::cout << "--------------------------------------------------------------------------------" << std::endl;

	getline(std::cin, choice);



	if (choice == "1")
	{
		system("cls");
		saveDatabaseLabel();
	}
	else if (choice == "2")
	{
		system("cls");
		deleteDatabaseLabel();
	}
	else if (choice == "3")
	{
		system("cls");
		listDatabaseStatus();
	}
	else if (choice == "4")
	{
		system("cls");
		tableMenu();
	}
	else if (choice == "5")
	{
		system("cls");
		readDataMenu();
	}
	else if (choice == "6")
	{
		system("cls");
		writeDataMenu();
	}
	else if (choice == "7")
	{
		system("cls");
		deleteDataMenu();
	}
	else if (choice == "P" || choice == "p")
	{
		if(Mix_PausedMusic() == 0)
			Mix_PauseMusic();
		else
			Mix_ResumeMusic();

			system("cls");
			mainMenu();
	}
	else if (choice == "S" || choice == "s")
	{

		system("cls");
		musicSettings();
	}
	else if (choice == "Q" || choice == "q")
	{

	}
	else
	{
		std::cout << choice << " is an invalid option! Try again..." << std::endl;
		Sleep(1000);
		system("cls");
		mainMenu();

	}
}
void musicSettings()
{
	

	std::cout << "---------------------------------MUSIC SETTINGS---------------------------------" << std::endl;
	std::cout << "Press + to increase the volume, Press - to decrease the volume." << std::endl;
	std::cout << "Press ENTER to choose a music file (including path).\nSYNTAX: C:\\Users\\Music\\song.mp3" << std::endl;
	std::cout << "Press ESC to return to main menu." << std::endl << std::endl;
	std::cout << "Currently playing: " << filePath << std::endl;
	std::cout << "--------------------------------------------------------------------------------" << std::endl;

	if (path == TPathType::PATH_NOT_SET)
	{

	}
	else if(path == TPathType::PATH_SUCCESS)
	{
		std::cout << "Successfully found " << filePath << std::endl;
		path = TPathType::PATH_NOT_SET;
	}
	else if(path == TPathType::PATH_FAILURE)
	{
		std::cout << "Could not find " << oldPath << std::endl;
		oldPath = TPathType::PATH_NOT_SET;
	}

	int c;
	
	do
	{
		c = getch();
		
		switch (c)
		{
		case '+':

			if (g_defaultVolume < 128)
			{
				Mix_VolumeMusic(g_defaultVolume++);
				std::cout << "Volume: " << g_defaultVolume << std::endl;
			}
			else
				std::cout << "Volume cannot be greater than 128" << std::endl;
			break;

		case '-':
			if (g_defaultVolume > 0)
			{
				Mix_VolumeMusic(g_defaultVolume--);
				std::cout << "Volume: " << g_defaultVolume << std::endl;
			}
			else
				std::cout << "Volume cannot be less than 0" << std::endl;
			break;
		case VK_RETURN:
			std::cout << "PATH: ";
			getline(std::cin, filePath);

	
			stopMusic(g_music);
				
				if (startMusic(g_music, filePath.c_str()))
				{
					path = TPathType::PATH_SUCCESS;
					system("cls");
					musicSettings();

				}	
				else
				{
					path = TPathType::PATH_FAILURE;
					oldPath = filePath;
					filePath = "defaultmusic.mp3";
					
					startMusic(g_music, defaultPath.c_str());

					system("cls");
					musicSettings();
				}
					
	
				break;
		case VK_ESCAPE:
					break;
		case VK_SPACE:
			std::cout << "SPACE OR BACKPSACE" << std::endl;
			break;
		default:
			std::cout << "Press + to increase volume, Press - to decrease volume" << std::endl;
			break;
		}
	} while (c != VK_ESCAPE);
	system("cls");
	mainMenu();

}

void tableMenu()
{
	bool bNameExists = false;

	std::string line;
	std::string choice;
	std::string tempConnectionString;

	fstream_IN.open("alldatabases.txt");
	while (getline(fstream_IN, line))
	{
		v_DatabaseLabels.push_back(line);
	}
	fstream_IN.close();

	if (v_DatabaseLabels.size() > 0)
	{

		std::cout << "------------------------------CREATE/DELETE TABLE----------------------------" << std::endl;
		std::cout << "Choose a database to create/delete a table from" << std::endl;
		std::cout << "Type cancel to return to main menu" << std::endl;
		std::cout << "--------------------------------------------------------------------------------" << std::endl;
		showDatabaseLabels();
		std::cout << "--------------------------------------------------------------------------------" << std::endl;

		do
		{
			getline(std::cin, choice);

			if (choice == "cancel")
			{
				v_DatabaseLabels.clear();
				system("cls");
				mainMenu();
			}

			for (int i = 0; i < v_DatabaseLabels.size(); i++)
			{
				if (choice == v_DatabaseLabels.at(i))
				{
					bNameExists = true;
				}
			}

			if (!bNameExists)
			{
				std::cout << "The database " << choice << " does not exist! Try again..." << std::endl;
			}
		} while (!bNameExists);

		if (connectToDatabase(choice))
		{
			do
			{

				system("cls");

				std::cout << "-----------------------------CONNECTED TO DATABASE------------------------------" << std::endl;
				std::cout << "Enter the option you wish to proceed with!" << std::endl;
				std::cout << "--------------------------------------------------------------------------------" << std::endl;
				std::cout << "1. Create table" << std::endl;
				std::cout << "2. Delete table" << std::endl;
				std::cout << "3. Back to main menu" << std::endl;
				std::cout << "--------------------------------------------------------------------------------" << std::endl;

				getline(std::cin, choice);

				if (choice == "1")
				{
					system("cls");
					std::string tableName;
					std::string columnChoice;

					std::cout << "----------------------------------CREATE TABLE----------------------------------" << std::endl << std::endl;
					std::cout << "CREATE SYNTAX" << std::endl;
					std::cout << "Create string: CREATE TABLE table_name" << std::endl;
					std::cout << "Attribute(s) string:" << std::endl;
					std::cout << "Primary key example: ID INT(10) AUTO_INCREMENT PRIMARY KEY," << std::endl;
					std::cout << "attribute_name variable_type(variable_max_size) optional_parameters," << std::endl;
					std::cout << "Example: FirstName VARCHAR(30) NOT NULL," << std::endl;
					std::cout << "--------------------------------------------------------------------------------" << std::endl;

					std::cout << "Enter the name of your table(s): ";
					getline(std::cin, tableName);
					std::cout << "Enter the attribute(s) (All in same string): ";
					getline(std::cin, columnChoice);

					createTable(tableName, columnChoice);



				}
				else if (choice == "2")
				{
					system("cls");
					std::string tableName;

					std::cout << "----------------------------------DELETE TABLE----------------------------------" << std::endl << std::endl;
					std::cout << "CREATE SYNTAX" << std::endl;
					std::cout << "Create string: CREATE TABLE table_name" << std::endl;
					std::cout << "Attribute(s) string:" << std::endl;
					std::cout << "Primary key example: ID INT(10) AUTO_INCREMENT PRIMARY KEY," << std::endl;
					std::cout << "attribute_name variable_type(variable_max_size) optional_parameters," << std::endl;
					std::cout << "Example: FirstName VARCHAR(30) NOT NULL," << std::endl;
					std::cout << "--------------------------------------------------------------------------------" << std::endl;

					std::cout << "Enter the name of your table(s): ";
					getline(std::cin, tableName);

					deleteTable(tableName);
				}
				else if (choice == "3")
				{




				}
				else
				{
					std::cout << choice << " is not a valid option! Try again..." << std::endl;
					Sleep(2000);
				}
			} while (choice != "3");

		}
		else
		{
			std::cout << "Could not connect do database!" << std::endl;
			Sleep(2000);
		}

	}
	else
	{
		std::cout << "------------------------------CREATE/DELETE TABLE----------------------------" << std::endl;
		std::cout << "Choose a database to create/delete a table from" << std::endl;
		std::cout << "Type cancel to return to main menu" << std::endl;
		std::cout << "--------------------------------------------------------------------------------" << std::endl;
		std::cout << "No databases found! Go create one." << std::endl;
		std::cout << "----------------------------------------------------------------------------" << std::endl;

		Sleep(2000);

	}

	system("cls");
	db.logoff();
	mainMenu();
}
void readDataMenu()
{
	bool bNameExists = false;

    std::string line;
	std::string choice;
	std::string tempConnectionString;

        fstream_IN.open("alldatabases.txt");
        while(getline(fstream_IN, line))
        {
            v_DatabaseLabels.push_back(line);
        }
        fstream_IN.close();

    if(v_DatabaseLabels.size() > 0)
    {


            std::cout << "-----------------------------------READ DATA------------------------------------" << std::endl;
            std::cout << "Choose a database to read data from" << std::endl;
            std::cout << "Type cancel to return to main menu" << std::endl;
            std::cout << "--------------------------------------------------------------------------------" << std::endl;
			showDatabaseLabels();
            std::cout << "--------------------------------------------------------------------------------" << std::endl;


			

			do
			{
				getline(std::cin, choice);

				if (choice == "cancel")
				{
					v_DatabaseLabels.clear();
					system("cls");
					mainMenu();
				}

				for (int i = 0; i < v_DatabaseLabels.size(); i++)
				{
					if (choice == v_DatabaseLabels.at(i))
					{
						bNameExists = true;
					}
				}

				if (!bNameExists)
				{
					std::cout << "The database " << choice << " does not exist! Try again..." << std::endl;
				}
			} while (!bNameExists);

			if (connectToDatabase(choice))
			{
				do
				{

					system("cls");

					std::cout << "-----------------------------CONNECTED TO DATABASE------------------------------" << std::endl;
					std::cout << "Enter the option you wish to proceed with!" << std::endl;
					std::cout << "--------------------------------------------------------------------------------" << std::endl;
					std::cout << "1. Read everything from chosen table(s)" << std::endl;
					std::cout << "2. Read chosen attributes from chosen table(s)" << std::endl;
					std::cout << "3. Custom SQL statement" << std::endl;
					std::cout << "4. Back to main menu" << std::endl;
					std::cout << "--------------------------------------------------------------------------------" << std::endl;

					getline(std::cin, choice);

					if (choice == "1")
					{
						system("cls");
						std::string tableChoice;


						std::cout << "------------------------------------READ ALL------------------------------------" << std::endl << std::endl;
						std::cout << "SELECT SYNTAX" << std::endl;
						std::cout << "Select string: SELECT * FROM persons" << std::endl;
						std::cout << "--------------------------------------------------------------------------------" << std::endl;

						std::cout << "Enter the name of your desired table: FROM ";
						getline(std::cin, tableChoice);

						selectAll(tableChoice);



					}
					else if (choice == "2")
					{
						system("cls");
						std::string selectChoice;
						std::string tableChoice;


						std::cout << "-----------------------------READ CHOSEN ATTRIBUTES-----------------------------" << std::endl << std::endl;
						std::cout << "SELECT SYNTAX" << std::endl;
						std::cout << "Select string: SELECT FirstName, LastName" << std::endl;
						std::cout << "From string: persons" << std::endl;;
						std::cout << "--------------------------------------------------------------------------------" << std::endl;

						std::cout << "Enter an or many attribute(s) or * to select everything: SELECT ";
						getline(std::cin, selectChoice);
						std::cout << "Enter the name of your desired table(s): FROM ";
						getline(std::cin, tableChoice);


						selectIndex(selectChoice, tableChoice);
					}
					else if (choice == "3")
					{
						system("cls");
						std::string SQLStatement;


						std::cout << "------------------------------CUSTOM SQL STATEMENT------------------------------" << std::endl << std::endl;
						std::cout << "NOTE: Expect unorganized text if multiple tables are chosen!" << std::endl;
						std::cout << "--------------------------------------------------------------------------------" << std::endl;

						std::cout << "Enter a SQL statement: SELECT ";
						getline(std::cin, SQLStatement);

						selectCustom(SQLStatement);



					}
					else if (choice == "4")
					{

					}
					else
					{
						std::cout << choice << " is not a valid option! Try again..." << std::endl;
						Sleep(2000);
					}
				} while (choice != "4");
			}
			else
			{
				std::cout << "Could not connect do database!" << std::endl;
				Sleep(2000);
			}
   

    }
    else
    {
        std::cout << "----------------------------------READ DATA---------------------------------" << std::endl << std::endl;
        std::cout << "Choose a database to read data from" << std::endl;
        std::cout << "Type cancel to return to main menu" << std::endl;
        std::cout << "----------------------------------------------------------------------------" << std::endl;
        std::cout << "No databases found! Go create one." << std::endl;
        std::cout << "----------------------------------------------------------------------------" << std::endl;

		Sleep(2000);

    }

	v_DatabaseLabels.clear();

	system("cls");
	db.logoff();
	mainMenu();
    
}
void writeDataMenu()
{
	

	bool bNameExists = false;

	std::string line;
	std::string choice;
	std::string tempConnectionString;

	fstream_IN.open("alldatabases.txt");
	while (getline(fstream_IN, line))
	{
		v_DatabaseLabels.push_back(line);
	}
	fstream_IN.close();

	if (v_DatabaseLabels.size() > 0)
	{




		std::cout << "-----------------------------------WRITE DATA-----------------------------------" << std::endl;
		std::cout << "Choose a database to write data to" << std::endl;
		std::cout << "Type cancel to return to main menu" << std::endl;
		std::cout << "--------------------------------------------------------------------------------" << std::endl;
		showDatabaseLabels();
		std::cout << "--------------------------------------------------------------------------------" << std::endl;

		do
		{
			getline(std::cin, choice);

			if (choice == "cancel")
			{
				v_DatabaseLabels.clear();
				system("cls");
				mainMenu();
			}

			for (int i = 0; i < v_DatabaseLabels.size(); i++)
			{
				if (choice == v_DatabaseLabels.at(i))
				{
					bNameExists = true;
				}
			}

			if (!bNameExists)
			{
				std::cout << "The database " << choice << " does not exist! Try again..." << std::endl;
			}
		} while (!bNameExists);

		if (connectToDatabase(choice))
		{
			do
			{

				system("cls");

				std::cout << "-----------------------------CONNECTED TO DATABASE------------------------------" << std::endl;
				std::cout << "Enter the option you wish to proceed with!" << std::endl;
				std::cout << "--------------------------------------------------------------------------------" << std::endl;
				std::cout << "1. Write to all indexes" << std::endl;
				std::cout << "2. Write to chosen indexes" << std::endl;
				std::cout << "3. Custom SQL statement" << std::endl;
				std::cout << "4. Back to main menu" << std::endl;
				std::cout << "--------------------------------------------------------------------------------" << std::endl;

				getline(std::cin, choice);

				if (choice == "1")
				{
					system("cls");
					std::string tableChoice;
					std::string values;


					std::cout << "-----------------------------------WRITE ALL------------------------------------" << std::endl << std::endl;
					std::cout << "INSERT SYNTAX" << std::endl;
					std::cout << "Insert string: INSERT INTO persons" << std::endl;
					std::cout << "Value string: 'Apple','Tomato', 18, 'Strawberry'" << std::endl;
					std::cout << "--------------------------------------------------------------------------------" << std::endl;

					std::cout << "Enter the name of your desired table(s): INSERT INTO ";
					getline(std::cin, tableChoice);
					std::cout << "Enter the values to the attribute(s): ";
					getline(std::cin, values);


					insertAll(tableChoice, values);



				}
				else if (choice == "2")
				{
					system("cls");
					std::string tableChoice;
					std::string columnChoice;
					std::string values;


					std::cout << "-----------------------------WRITE CHOSEN ATTRIBUTES----------------------------" << std::endl << std::endl;
					std::cout << "INSERT SYNTAX" << std::endl;
					std::cout << "Insert string: INSERT INTO persons" << std::endl;
					std::cout << "Attribute string: Fruit, Vegetable, Age, Berry" << std::endl;
					std::cout << "Value string: 'Apple','Tomato', 18, 'Strawberry'" << std::endl;
					std::cout << "--------------------------------------------------------------------------------" << std::endl;

					std::cout << "Enter the name of your desired table(s): INSERT INTO ";
					getline(std::cin, tableChoice);
					std::cout << "Enter the attribute(s): ";
					getline(std::cin, columnChoice);
					std::cout << "Enter the values to the attribute(s): ";
					getline(std::cin, values);


					insertIndex(tableChoice, columnChoice, values);
				}
				else if (choice == "3")
				{
					system("cls");
					std::string SQLStatement;


					std::cout << "------------------------------CUSTOM SQL STATEMENT------------------------------" << std::endl << std::endl;

					std::cout << "Enter a SQL statement: INSERT INTO ";
					getline(std::cin, SQLStatement);

					insertCustom(SQLStatement);



				}
				else if (choice == "4")
				{

				}
				else
				{
					std::cout << choice << " is not a valid option! Try again..." << std::endl;
					Sleep(2000);
				}
			} while (choice != "4");

		}
		else
		{
			std::cout << "Could not connect do database!" << std::endl;
			Sleep(2000);
		}

	}
	else
	{
		std::cout << "-----------------------------------WRITE DATA-----------------------------------" << std::endl;
		std::cout << "Choose a database to write data to" << std::endl;
		std::cout << "Type cancel to return to main menu" << std::endl;
		std::cout << "--------------------------------------------------------------------------------" << std::endl;
		std::cout << "No databases found! Go create one." << std::endl;
		std::cout << "----------------------------------------------------------------------------" << std::endl;

		Sleep(2000);

	}

	system("cls");
	db.logoff();
	mainMenu();
}
void deleteDataMenu()
{

	bool bNameExists = false;

	std::string line;
	std::string choice;
	std::string tempConnectionString;

	fstream_IN.open("alldatabases.txt");
	while (getline(fstream_IN, line))
	{
		v_DatabaseLabels.push_back(line);
	}
	fstream_IN.close();

	if (v_DatabaseLabels.size() > 0)
	{

		std::cout << "-----------------------------------DELETE DATA----------------------------------" << std::endl;
		std::cout << "Choose a database to delete data from" << std::endl;
		std::cout << "Type cancel to return to main menu" << std::endl;
		std::cout << "--------------------------------------------------------------------------------" << std::endl;
		showDatabaseLabels();
		std::cout << "--------------------------------------------------------------------------------" << std::endl;

		do
		{
			getline(std::cin, choice);

			if (choice == "cancel")
			{
				v_DatabaseLabels.clear();
				system("cls");
				mainMenu();
			}

			for (int i = 0; i < v_DatabaseLabels.size(); i++)
			{
				if (choice == v_DatabaseLabels.at(i))
				{
					bNameExists = true;
				}
			}

			if (!bNameExists)
			{
				std::cout << "The database " << choice << " does not exist! Try again..." << std::endl;
			}
		} while (!bNameExists);

		if (connectToDatabase(choice))
		{
			do
			{

				system("cls");

				std::cout << "-----------------------------CONNECTED TO DATABASE------------------------------" << std::endl;
				std::cout << "Enter the option you wish to proceed with!" << std::endl;
				std::cout << "--------------------------------------------------------------------------------" << std::endl;
				std::cout << "1. Delete indexes" << std::endl;
				std::cout << "2. Custom SQL statement" << std::endl;
				std::cout << "3. Back to main menu" << std::endl;
				std::cout << "--------------------------------------------------------------------------------" << std::endl;

				getline(std::cin, choice);

				if (choice == "1")
				{
					system("cls");
					std::string tableChoice;
					std::string columnChoice;
					std::string values;


					std::cout << "----------------------------------DELETE ROWS-----------------------------------" << std::endl << std::endl;
					std::cout << "DELETE SYNTAX" << std::endl;
					std::cout << "Delete string: DELETE FROM persons" << std::endl;
					std::cout << "Where string: WHERE City = Vasa" << std::endl;
					std::cout << "Delete all from table: DELETE * FROM persons" << std::endl;
					std::cout << "--------------------------------------------------------------------------------" << std::endl;

					std::cout << "Enter the name of your desired table(s): DELETE FROM ";
					getline(std::cin, tableChoice);
					std::cout << "Enter the attribute(s): ";
					getline(std::cin, columnChoice);
					std::cout << "Enter the values to the attribute(s): ";
					getline(std::cin, values);


					deleteIndex(tableChoice, columnChoice, values);



				}
				else if (choice == "2")
				{
					system("cls");
					std::string SQLStatement;


					std::cout << "------------------------------CUSTOM SQL STATEMENT------------------------------" << std::endl << std::endl;

					std::cout << "Enter a SQL statement: DELETE ";
					getline(std::cin, SQLStatement);

					deleteCustom(SQLStatement);
				}
				else if (choice == "3")
				{




				}
				else
				{
					std::cout << choice << " is not a valid option! Try again..." << std::endl;
					Sleep(2000);
				}
			} while (choice != "3");

		}
		else
		{
			std::cout << "Could not connect do database!" << std::endl;
			Sleep(2000);
		}

	}
	else
	{
		std::cout << "-----------------------------------DELETE DATA----------------------------------" << std::endl;
		std::cout << "Choose a database to delete data from" << std::endl;
		std::cout << "Type cancel to return to main menu" << std::endl;
		std::cout << "--------------------------------------------------------------------------------" << std::endl;
		std::cout << "No databases found! Go create one." << std::endl;
		std::cout << "----------------------------------------------------------------------------" << std::endl;

		Sleep(2000);

	}

	system("cls");
	db.logoff();
	mainMenu();
}

void showDatabaseLabels()
{
	std::string line;
	fstream_IN.open("alldatabases.txt");
	while (getline(fstream_IN, line))
	{
		std::cout << line << std::endl;
	}
	fstream_IN.close();
}

void createTable(std::string &p_table, std::string &p_attributes)
{
	try
	{

		std::string insertString;

		insertString = "CREATE TABLE " + p_table + " (" + p_attributes + ")";


		otl_stream dbStream;

		system("cls");
		std::cout << "------------------------------------TABLE(S)------------------------------------" << std::endl;
		std::cout << p_table << std::endl;
		std::cout << "-----------------------------------ATTRIBUTES-----------------------------------" << std::endl;
		std::cout << p_attributes << std::endl;

		dbStream.open(100, insertString.c_str(), db);

		std::cout << "================================================================================" << std::endl;
		std::cout << std::endl << "Successfully created table(s) " << p_table << " with attributes " << p_attributes << std::endl;
		dbStream.close();
		std::cout << std::endl << "**************************************DONE**************************************" << std::endl << std::endl;




	}
	catch (otl_exception &e)
	{
		std::cout << "Syntax error/Table not found/Connection lost!" << std::endl;
	}

	system("PAUSE");
}
void deleteTable(std::string &p_table)
{
	try
	{

		std::string insertString;

		insertString = "DROP TABLE " + p_table;


		otl_stream dbStream;

		system("cls");
		std::cout << "------------------------------------TABLE(S)------------------------------------" << std::endl;
		std::cout << p_table << std::endl;

		dbStream.open(100, insertString.c_str(), db);

		std::cout << "================================================================================" << std::endl;
		std::cout << std::endl << "Successfully deleted table(s) " << p_table << std::endl;
		dbStream.close();
		std::cout << std::endl << "**************************************DONE**************************************" << std::endl << std::endl;




	}
	catch (otl_exception &e)
	{
		std::cout << "Syntax error/Table not found/Connection lost!" << std::endl;
	}

	system("PAUSE");
}

void selectAll(std::string &p_table)
{
	try
	{

		std::string selectString;
		std::string line;
		std::string temp;

		int desc_len;
		int rpc = 0;
		bool bReachedMaxLines = false;





		selectString = "SELECT * FROM " + p_table;


		otl_stream dbStream;




		dbStream.set_all_column_types(otl_all_num2str);

		dbStream.open(100, selectString.c_str(), db);

		otl_column_desc* desc;
		desc = dbStream.describe_select(desc_len);
		system("cls");
		std::cout << "------------------------------------TABLE(S)------------------------------------" << std::endl;
		std::cout << p_table << std::endl;
		std::cout << "--------------------------------SELECT STATEMENT--------------------------------" << std::endl;
		std::cout << "*" << std::endl;

		for (int i = 1; !dbStream.eof(); i++)
		{


			temp = std::to_string(i);

			std::cout << "=======================================" << i; for (int i = 0; i < (41 - temp.length()); i++){ std::cout << "="; } std::cout << std::endl;
			for (int col_num = 0; col_num < desc_len; ++col_num)
			{
				rpc++;
				if (rpc > 15000)
				{
					bReachedMaxLines = true;
				}

					dbStream >> line;
					if (dbStream.is_null())
						std::cout << desc[col_num].name << ": NULL" << std::endl;
					else
						std::cout << desc[col_num].name << ": " << line << std::endl;


			}

			if (bReachedMaxLines)
			{
				bReachedMaxLines = false;
				rpc = 0;
				std::cout << std::endl << "You have reached the maximum amount of lines, get what information you need." << std::endl << std::endl;
				system("PAUSE");
				system("cls");
			}


		}

		dbStream.close();
		std::cout << std::endl << "**************************************DONE**************************************" << std::endl << std::endl;

	}
	catch (otl_exception &e)
	{
		std::cout << "Syntax error/Table not found/Connection lost!" << std::endl;

	}

	system("PAUSE");
}
void selectIndex(std::string &p_select, std::string &p_table)
{
	try
	{

		std::string selectString;
		std::string line;
		std::string temp;

		int desc_len;
		int rpc = 0;
		bool bReachedMaxLines = false;

		selectString = "SELECT " + p_select + " FROM " + p_table;



		otl_stream dbStream;




		dbStream.set_all_column_types(otl_all_num2str);

		dbStream.open(100, selectString.c_str(), db);

		otl_column_desc* desc;
		desc = dbStream.describe_select(desc_len);
		system("cls");

		std::cout << "------------------------------------TABLE(S)------------------------------------" << std::endl;
		std::cout << p_table << std::endl;
		std::cout << "--------------------------------SELECT STATEMENT--------------------------------" << std::endl;
		std::cout << p_select << std::endl;

		
		for (int i = 1; !dbStream.eof(); i++)
		{
			
			temp = std::to_string(i);

			std::cout << "=======================================" << i; for (int i = 0; i < (41 - temp.length()); i++){ std::cout << "="; } std::cout << std::endl;
			for (int col_num = 0; col_num < desc_len; ++col_num)
			{
				rpc++;
				if (rpc > 15000)
				{
					bReachedMaxLines = true;
				}

					dbStream >> line;
					if (dbStream.is_null())
						std::cout << desc[col_num].name << ": NULL" << std::endl;
					else
						std::cout << desc[col_num].name << ": " << line << std::endl;
				

			}

			if (bReachedMaxLines)
			{
				bReachedMaxLines = false;
				rpc = 0;
				std::cout << std::endl << "You have reached the maximum amount of lines, get what information you need." << std::endl << std::endl;
				system("PAUSE");
				system("cls");
			}

		}



		std::cout << std::endl << "**************************************DONE**************************************" << std::endl << std::endl;

		dbStream.close();

	}
	catch (otl_exception &e)
	{
		std::cout << "Syntax error/Table not found/Connection lost!" << std::endl;
	}

	system("PAUSE");
}
void selectCustom(std::string &p_SQL)
{
	try
	{

		std::string line;
		std::string selectString;
		std::string temp;

		int desc_len;
		int rpc = 0;
		bool bReachedMaxLines = false;




		otl_stream dbStream;


		selectString = "SELECT " + p_SQL;

		dbStream.set_all_column_types(otl_all_num2str);

		dbStream.open(100, selectString.c_str(), db);

		otl_column_desc* desc;
		desc = dbStream.describe_select(desc_len);
		system("cls");

		std::cout << "------------------------------CUSTOM SQL STATEMENT------------------------------" << std::endl << std::endl;
		std::cout << "SELECT " << p_SQL << std::endl;

		for (int i = 1; !dbStream.eof(); i++)
		{
			temp = std::to_string(i);

			std::cout << "=======================================" << i; for (int i = 0; i < (41 - temp.length()); i++){ std::cout << "="; } std::cout << std::endl;
			
			for (int col_num = 0; col_num < desc_len; ++col_num)
			{
				rpc++;
				if (rpc > 15000)
				{
					bReachedMaxLines = true;
				}

				dbStream >> line;
				if (dbStream.is_null())
					std::cout << desc[col_num].name << ": NULL" << std::endl;
				else
					std::cout << desc[col_num].name << ": " << line << std::endl;


			}

			if (bReachedMaxLines)
			{
				bReachedMaxLines = false;
				rpc = 0;
				std::cout << std::endl << "You have reached the maximum amount of lines, get what information you need." << std::endl << std::endl;
				system("PAUSE");
				system("cls");
			}

		}



		std::cout << std::endl << "**************************************DONE**************************************" << std::endl << std::endl;

		dbStream.close();

	}
	catch (otl_exception &e)
	{
		std::cout << "Syntax error/Table not found/Connection lost!" << std::endl;

	}

	system("PAUSE");
}

void insertAll(std::string &p_table, std::string &p_values)
{
	try
	{

		std::string insertString;

		insertString = "INSERT INTO " + p_table + " VALUES (" + p_values + ");";


		otl_stream dbStream;

		system("cls");
		std::cout << "------------------------------------TABLE(S)------------------------------------" << std::endl;
		std::cout << p_table << std::endl;
		std::cout << "-------------------------------------VALUES-------------------------------------" << std::endl;
		std::cout << p_values << std::endl;

		dbStream.open(100, insertString.c_str(), db);

		std::cout << "================================================================================" << std::endl;
		std::cout << std::endl << "Successfully inserted value(s) " << p_values << " into " << p_table;
		dbStream.close();
		std::cout << std::endl << "**************************************DONE**************************************" << std::endl << std::endl;


		

	}
	catch (otl_exception &e)
	{
		std::cout << "Syntax error/Table not found/Connection lost!" << std::endl;
		std::cout << "Check the attribute(s) variable type(s)! Remove quotes if the type is a number." << std::endl;
	}

	system("PAUSE");
}
void insertIndex(std::string &p_table, std::string &p_columns, std::string &p_values)
{
	try
	{

		std::string insertString;

		insertString = "INSERT INTO " + p_table + " (" + p_columns + ") " + "VALUES (" + p_values + ");";

		otl_stream dbStream;

		system("cls");
		std::cout << "------------------------------------TABLE(S)------------------------------------" << std::endl;
		std::cout << p_table << std::endl;
		std::cout << "-----------------------------------ATTRIBUTES-----------------------------------" << std::endl;
		std::cout << p_columns << std::endl;
		std::cout << "-------------------------------------VALUES-------------------------------------" << std::endl;
		std::cout << p_values << std::endl;

		dbStream.open(100, insertString.c_str(), db);

		std::cout << "================================================================================" << std::endl;
		std::cout << std::endl << "Successfully inserted value(s) " << p_values << " into " << p_table;
		dbStream.close();
		std::cout << std::endl << "**************************************DONE**************************************" << std::endl << std::endl;




	}
	catch (otl_exception &e)
	{
		std::cout << "Syntax error/Table not found/Connection lost!" << std::endl;
		std::cout << "Check the attribute(s) variable type(s)! Remove quotes if the type is a number." << std::endl;
	}

	system("PAUSE");
}
void insertCustom(std::string &p_SQL)
{
	try
	{

		std::string insertString;

		otl_stream dbStream;

		insertString = "INSERT " + p_SQL;

		system("cls");
		std::cout << "------------------------------CUSTOM SQL STATEMENT------------------------------" << std::endl << std::endl;
		std::cout << "INSERT " << p_SQL << std::endl;


		

		dbStream.open(100, insertString.c_str(), db);

		std::cout << "================================================================================" << std::endl;
		std::cout << std::endl << "Successfully executed " << p_SQL << std::endl;
		dbStream.close();
		std::cout << std::endl << "**************************************DONE**************************************" << std::endl << std::endl;




	}
	catch (otl_exception &e)
	{
		std::cout << "Syntax error/Table not found/Connection lost!" << std::endl;
		std::cout << "Check the attribute(s) variable type(s)! Remove quotes if the type is a number." << std::endl;
	}

	system("PAUSE");
}

void deleteIndex(std::string &p_table, std::string &p_columns, std::string &p_values)
{
	try
	{

		std::string insertString;

		insertString = "DELETE FROM " + p_table + " WHERE " + p_columns + " = " + p_values;


		otl_stream dbStream;

		system("cls");
		std::cout << "------------------------------------TABLE(S)------------------------------------" << std::endl;
		std::cout << p_table << std::endl;
		std::cout << "-----------------------------------ATTRIBUTES-----------------------------------" << std::endl;
		std::cout << p_columns << std::endl;
		std::cout << "-------------------------------------VALUES-------------------------------------" << std::endl;
		std::cout << p_values << std::endl;

		dbStream.open(100, insertString.c_str(), db);

		std::cout << "================================================================================" << std::endl;
		std::cout << std::endl << "Successfully deleted row(s) where " << p_columns << " = " << p_values;
		dbStream.close();
		std::cout << std::endl << "**************************************DONE**************************************" << std::endl << std::endl;




	}
	catch (otl_exception &e)
	{
		std::cout << "Syntax error/Table not found/Connection lost!" << std::endl;
		std::cout << "Check the attribute(s) variable type(s)! Remove quotes if the type is a number." << std::endl;
	}

	system("PAUSE");
}
void deleteCustom(std::string &p_SQL)
{
	try
	{

		std::string insertString;

		insertString = "DELETE " + p_SQL;


		otl_stream dbStream;

		system("cls");
		std::cout << "------------------------------CUSTOM SQL STATEMENT------------------------------" << std::endl << std::endl;
		std::cout << "DELETE " << p_SQL << std::endl;

		dbStream.open(100, insertString.c_str(), db);

		std::cout << "================================================================================" << std::endl;
		std::cout << std::endl << "Successfully executed " << p_SQL << std::endl;
		dbStream.close();
		std::cout << std::endl << "**************************************DONE**************************************" << std::endl << std::endl;




	}
	catch (otl_exception &e)
	{
		std::cout << "Syntax error/Table not found/Connection lost!" << std::endl;
		std::cout << "Check the attribute(s) variable type(s)! Remove quotes if the type is a number." << std::endl;
	}

	system("PAUSE");
}

void stringLoop(std::string string, int milliseconds)
{
	for (int i = 0; i < string.length(); i++)
	{
		std::cout << string.at(i);
		Sleep(milliseconds);
	}
}

bool startMusic(Mix_Music* p_music, std::string p_musicfile)
{
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);

	
	p_music = Mix_LoadMUS(p_musicfile.c_str());
	Mix_VolumeMusic(10);

	if (Mix_PlayMusic(p_music, -1) == 0)
		return true;
	else
		return false;

	
}
void stopMusic(Mix_Music* p_music)
{
	Mix_FreeMusic(p_music);
	Mix_CloseAudio();
}