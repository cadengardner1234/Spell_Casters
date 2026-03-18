Spell-Casters User Manual

**THIS GAME WAS CREATED BY CADEN GARDNER, ZACH WOOD, MADDY TEER, TIMOTHY ACKLIN, AND DREW BALDUCCI AS A FINAL PROJECT FOR CPE 353 AT UAH**
**THE ORIGINAL REPOSITORY HAS BEEN DELETED, SO I FOUND A BACKUP THAT IS THE VERSION I UPLOADED TO GITLAB. THIS VERSION MAY NOT FUNCTION AS INTENDED AND MAY HAVE
BUGS THAT WERE PATCHED IN THE LATER, NOW DELETED VERSION**

Instructions for Compilation:
	The user must have all files within the two provided project groupings: server and client. Each project file must be compiled and run separately, however both are required to support multiplayer functionality. 

Steps for compiling and running the game:
1. Download the provided server and client project files 
2. Open the two projects within QTCreator 
** When running the game it is best practice to run the server project first

After opening and verifying that both projects compile as intended you are ready to play the game. 


Instructions for Game Execution:
	In order to run Spell-Casters, you must be on an ENG246 Linux laboratory machine. Make sure your machine has QTCreator installed. You need one machine to act as the server. This machine can also run a client at the same time. On the designated server machine, run the “server” project using QTCreator. A blank dialog box should appear with the server’s IP address written at the top. Now, on up to 4 different machines, run the “client” project using QTCreator. You should see a main menu with a box at the bottom that you can enter the server’s IP address into. Click the “Connect” button at the bottom of the main menu. If a valid IP is entered, you should now find yourself in a pre-game lobby. When you are ready, press the “Start” button to be put into the game.




Game Objectives:
	The objective of Spell-Casters is to be the last player standing among 2-4 players, or to simply practice casting spells in single-player mode. Once in a game, eliminating players is as simple as aiming, and casting a spell in their direction by typing it. To survive, cast shields and heals to maintain your hp and block damage. Dying before any other player is counted as a loss. Living when every other player is dead counts as a win. 

Controls:
Players can use either WASD or IJKL to move their crosshair around the gamescreen.
Pressing Enter will swap the player between typing mode and aiming mode.
To cast a spell, the player will need to type the name of the desired spell into the line edit and press Enter.
 View all spellnames in the Spellbook non-modal Spellbook dialog that appears after starting the game.
For attacks, the spell will be sent in the direction of the crosshair from your player, starting at your crosshair’s location.
When casting a defense spell, it will be placed at the location of your crosshair. Defenses will block one attack before they despawn.
When casting a Healing spell, location of the cursor does not matter and health will be applied automatically;



Additional Information:
Troubleshooting: 
If a client is having connection issues ensure that you are trying to connect from a ENG246 machine and the correct IP is entered when trying to connect
Cross network connection is not supported: ensure all machines are connected via the same network
If QTCreator is giving a build error ensure that all project files listed in the dynamic analysis are present and up to date 

Limitations:
Our game only supports up to 4 clients in one game 
After the first player starts the game, no new players may join the game
Single Player is used for targeting practice and will not update any wins or losses
