#Nom des programme
CLIENT = client
BOARD = board

#Compilation
CC=gcc

#Options de compilation
CFLAGS=-Wall
#Options d edition de liens
LDFLAGS=-lm -pthread

#Repertoire des sources et entete du projet partie client
clientdir=./
boarddir=../board/
#Repertoire des sources et entete des fichiers utilitaires
utilsdir=../utils/
#Repertoire qui contient les fichiers objets et executable
bindir=../output/

#Fichiers source
SRC_CLIENT=$(wildcard $(clientdir)*.c)
SRC_BOARD=$(wildcard $(boarddir)*.c)
SRC_UTILS=$(wildcard $(utilsdir)*.c)
#Fichiers objet
# OBJ_CLIENT=$(subst $(clientdir), $(bindir),$(SRC_CLIENT:.c=.o))
# OBJ_BOARD=$(subst $(boarddir), $(bindir),$(SRC_BOARD:.c=.o))
OBJ_UTILS=$(subst $(utilsdir), $(bindir),$(SRC_UTILS:.c=.o))
OBJ_CLIENT=$(SRC_CLIENT:.c=.o)
OBJ_BOARD=$(SRC_BOARD:.c=.o)

#Commande pour effacer des fichiers
RM=find .. -name "*.bak" -type f -delete && find .. -name "*.o" -type f -delete && find .. -name "*.old" -type f -delete && find .. -name "*~" -type f -delete

all : $(CLIENT) $(BOARD)
	make clean

$(CLIENT) : $(OBJ_CLIENT) $(OBJ_UTILS)
	$(CC) $^ -o $(bindir)$@ $(LDFLAGS)
	find ${clientdir} -name "*.o" -type f -delete

$(BOARD) : $(OBJ_BOARD) $(OBJ_UTILS)
	$(CC) $^ -o $(bindir)$@ $(LDFLAGS)
	find ${boarddir} -name "*.o" -type f -delete

../output/%.o : ./%.c
	$(CC) $(CFLAGS) -c $< -o $@

../output/%.o : ../utils/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : clean execute

clean : 
	$(RM)

#Executer le programme
execute :
	@$(bindir)$(CLIENT)