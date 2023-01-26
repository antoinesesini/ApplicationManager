//
// Created by Antoine Sesini on 21/12/2022.
//
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> /* Types pid_t... */
#include <unistd.h> /* fork()... */
#include <sys/wait.h>//waitpid
#include <string.h>
#include <ctype.h>

int stop_pid;


void handler(int n, siginfo_t *info, void *ptr)
{
    int sender_pid = info->si_pid;
    //Si on reçoit le SIGUSR1 ALORS ON PROVOQUE L'ARRET DE TOUTES LES APPLICATIONS
    if (n==SIGUSR1 && sender_pid==stop_pid)
    {
        kill(-getpid(),SIGTERM); //Arrêt de toutes les applications du groupe
    }

    //Si on reçoit un SIGTERM ALORS ON ARRETE LE MANAGER
    if (n==SIGTERM)
    {
        printf("Fermeture de l'ApplicationManager\n");
    }
}





int my_system(char *path, char **argv){
    execv(path, argv);
    perror("execv");
    return EXIT_FAILURE;
}




void lancerApp(char* nomApp,char* path, int nbArguments)
{
    /*LANCER UNE APPLICATION REVIENT A :
     * Construire la commande de lancement pour utiliser execv (afin d'avoir un fils direct)
     * Sous traiter l'execution à l'aide d'un processus fils (fork)
     * Une fois que le processus est créé, redéfinir le comportement du processus lors de la réception des signals SIGCHLD et SIGUSR1 (fonction signal)
     * Lancer l'execution par le fils
     *
     * */
    printf("LANCEMENT DE : %s...\n",path);
    //Découpage de la commande
    char** commande= malloc(sizeof(char*)*20);
    char d[]=" ";
    commande[0]= strtok(path,d);
    for (int it=1; it<=nbArguments;it++)
    {
        commande[it]= strtok(NULL,d);
    }



    pid_t pid;
    char * stopProgram="Power Manager";

    //EXECUTION COMMANDE !!
    pid=fork();

    if (pid==-1)
        printf("L'application ne peut être lancée : erreur de creation de processus.");
    else
    {
        if(pid == 0){
            //On est dans le fils
            my_system(commande[0],commande);
        }
        else
        {
            if (!strcmp(nomApp,stopProgram))
                stop_pid=pid;
        }
    }


}













void main(){
    char* compilation="gcc -c *.c -w";
    system(compilation);
    char* linkage="gcc power_manager.o -o power_manager -w";
    system(linkage);
    linkage="gcc network_manager.o -o network_manager -w";
    system(linkage);
    linkage="gcc get_time.o -o get_time -w";
    system(linkage);
    linkage="killall -q power_manager";
    system(linkage);
    linkage="killall -q network_manager";
    system(linkage);
    linkage="killall -q get_time";
    system(linkage);

    pid_t wpid;
    int status;


    struct sigaction sig_handler;
    sig_handler.sa_sigaction=handler;
    sig_handler.sa_flags=SA_SIGINFO;
    //Redéfinition du sigusr1 reçu par le power_manager
    int r=sigaction(SIGUSR1,&sig_handler,NULL);
    if (r<0){perror("sigaction"); exit(EXIT_FAILURE);}
    //Redéfinition du sigterm reçu par lui même
    r=sigaction(SIGTERM,&sig_handler,NULL);
    if (r<0){perror("sigaction"); exit(EXIT_FAILURE);}

    /*DEMARCHE DE LA FONCTION PRINCIPALE
     * Création des variables nécessaires
     * Ouverture du fichier
     * Récupération du nombre d'applications sur la première ligne
     * Pour chaque appli, on récupère son nom, son path, son nombre d'arguments et ses arguments s'il y en a
     * On lance l'application dans un fork
     * La fonction lancer() qui lance les applications se charge de redéfinir les traitements des signaux
     * */
    //Création des variables nécessaires
    FILE* fichier;
    char currentChar;
    int nbApp=0;
    int returnValue;


    //Ouverture du fichier
    fichier=fopen("list_appli.txt","r");

    if (fichier == NULL){
        perror ("Le fichier n'a pu s'ouvrir !\n");
        exit(-1);
    }


    //Récupération du nombre d'applications sur la première ligne
    while(currentChar != '=' && !feof(fichier)){
        returnValue = fread(&currentChar,1,1,fichier);
        if (!returnValue)
            printf("La lecture n'a rien donné\n");
    }
    do {
        returnValue = fread(&currentChar,1,1,fichier);
        if (!returnValue)
            printf("La lecture n'a rien donné\n");
        if (isdigit(currentChar))
            nbApp=(nbApp*10)+atoi(&currentChar);
    }
   while (isdigit(currentChar));
   printf("NOMBRE D'APPLICATIONS A LANCER : %d\n\n", nbApp);




    char* testReturnValue;
    int cpt=0;
    char chaineRecup[100];
    char *debutScan;
    int cptAttribut=0;
    char* nomApp;
    char* pathApp;
    int nbArguments=0;
    //Pour chaque appli, on récupère son nom, son path, son nombre d'arguments et ses arguments s'il y en a
    while (!feof(fichier) && cpt<nbApp)//Pour chaque appli
    {
        while (cptAttribut<4)
        {
            testReturnValue=fgets(chaineRecup, 100, fichier);

            if (!testReturnValue)
                printf("La lecture n'a rien donné\n");

            debutScan= strchr(chaineRecup,'=');
            if (debutScan!=NULL)
            {
                debutScan++;
                switch(cptAttribut)
                {
                    case 0:
                        //RECUPERATION DU NOM DE L'APPLICATION
                        nomApp= malloc(sizeof(char)*strlen(debutScan));
                        for (int i = 0; i<strlen(debutScan)-2;i++){ //On fait -2 pour enlever les sauts de lignes et le caracère de fin de chaine
                            nomApp[i] = debutScan[i];
                        }
                        break;
                    case 1:
                        //RECUPERATION DU PATH DE L'APPLICATION
                        pathApp= malloc(sizeof(char)*strlen(debutScan));
                        for (int j = 0; j<strlen(debutScan)-2;j++){
                            pathApp[j] = debutScan[j];
                        }
                        break;
                    case 2:
                        //RECUPERATION DU NOMBRE D'ARGUMENTS DE L'APPLICATION
                        nbArguments= atoi(debutScan);
                        break;
                    case 3:
                        //RECUPERATION DES ARGUMENTS DE L'APPLICATION + ENCHAINEMENT SUR LE LANCEMENT DES APPLIS
                        if (nbArguments>0) //Si il y a des arguments, alors on remplie la commande
                        {
                            char *longPath = malloc(sizeof(char)*200);
                            strcpy(longPath,pathApp);
                            for (int k=0;k<nbArguments;k++) //Pour chaque arguments, on ajoute un separateur puis l'argument ensuite
                            {
                                char *espace = " ";
                                strcat(longPath, espace);
                                char* currentArgument= malloc(sizeof(char)*(50));
                                fgets(chaineRecup,100,fichier);
                                for(int m = 0; m<strlen(chaineRecup)-2;m++){ //On applique -2 car les retours du fichiers sont au format windows
                                    currentArgument[m] = chaineRecup[m];
                                }
                                strcat(longPath,currentArgument);
                            }
                            lancerApp(nomApp,longPath, nbArguments);
                        }
                        else
                            lancerApp(nomApp, pathApp, nbArguments);
                        break;
                    default:
                        break;
                }
                cptAttribut++;
            }
        }
        cptAttribut=0;
        cpt++;
    }

    printf("\nLOG :\n");
    while ((wpid = wait(&status)) > 0);

    fclose(fichier);

    exit(EXIT_SUCCESS);
}