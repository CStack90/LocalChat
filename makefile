#Nom des programme
CLIENT = client
SERVER = server

#Repertoire des sources et entete du projet partie client
clientdir=client
#Repertoire des sources et entete du projet partie serveur
serverdir=server

#Repertoire qui contient les fichiers objets et executable
bindir=./output/

#Commande pour effacer des fichiers
RM=find . -name "*.bak" -type f -delete && find . -name "*.o" -type f -delete && find . -name "*.old" -type f -delete && find . -name "*~" -type f -delete

all : compile

compile : 
	cd $(clientdir) && $(MAKE)
	cd $(serverdir) && $(MAKE)

.PHONY : clean server client board

clean : 
	$(RM)

#Executer le programme
server :
	@$(bindir)$(SERVER)
client :
	@$(bindir)$(CLIENT)
