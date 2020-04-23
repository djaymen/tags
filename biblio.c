#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <fcntl.h>
#include <string.h>
#include "biblio.h"

///  -----------------------------------------------------------------------------------
int isExistTag(Tags tags, char *tagName)
{

    /* get the first token */
    Token *token = tags.sommet;

    /* walk through other tokens */
    while (token != NULL)
    {
        if (strcmp(token->tag, tagName) == 0)
            return 1;
        token = token->suivant;
    }
    return 0;
}

void set_tags(char *Path, char data[MAXLEN], char *tagName, int replace)
{
    char buf[30];
    sprintf(buf, "user.%s", tagName);
    if (replace == 0)
    {
        char tab[MAXLEN] = "";
        if (setxattr(Path, buf, tab, sizeof(tab), 0) > -1)
        {
            printf("tag set\n");
        }
        else
        {
            perror(" err set");
        }
    }
    else
    { /// replace data

        //removexattr(Path,tagName);
        char tab[MAXLEN];

        int i;
        for (i = 0; i < strlen(data); i++)
        {
            printf("c=%c\n", data[i]);
            tab[i] = data[i];
        }
        tab[i] = '\0';
        if (setxattr(Path, buf, tab, sizeof(tab), XATTR_REPLACE) > -1)
        {
            printf("tag modifier\n");
        }
        else
        {
            perror(" err set");
        }
    }
}
void add(Tags *tags, char *tagName)
{
    Token *token = malloc(sizeof(Token));
    strcpy(token->tag, tagName);
    token->suivant = tags->sommet;
    tags->sommet = token;
    tags->NbTags += 1;
    return;
}
void deletTag(Tags *tags, char *tagName)
{
    Token *token = tags->sommet;
    Token *pres = token;
    int find = 0;
    while (find == 0 && token != NULL)
    {
        if (strcmp(token->tag, tagName) == 0)
        {
            find = 1;
        }
        else
        {
            pres = token;
            token = token->suivant;
        }
    }
    if (find)
    {
        tags->NbTags -= 1;
        if (pres == token)
            tags->sommet = token->suivant;
        else
            pres->suivant = token->suivant;
    }
}
void init_tags(Tags *tags, char buf[MAXLEN])
{
    const char s[2] = "#";
    char *token;
    if (buf != "")
    {
        /* get the first token */
        token = strtok(buf, s);

        /* walk through other tokens */
        while (token != NULL)
        {
            add(tags, token);
            token = strtok(NULL, s);
        }
    }
}
void get_tags(Tags *tags, char *Path, char *tagName)
{

    const char *fichier = Path;
    char buf_Tags[MAXLEN];
    char buf[30];
    sprintf(buf, "user.%s", tagName);
    if (getxattr(fichier, buf, buf_Tags, sizeof(buf_Tags)))
    {
        init_tags(tags, buf_Tags);
    }
    else
    {
        printf("Error");
    }
}
void ListOfTags(Tags *tableau, char buff[MAXLEN], int size)
{

    int index = 0;
    while (index < (size - 1))
    {
        char *mot = &buff[index];
        char *m = &mot[5];
        // printf("mot =%s and %d\n",m,strlen(m));
        add(tableau, m);
        index += strlen(m) + 6;
    }
}
int findInList(Tags *tags, char *tagName)
{
    Token *token = tags->sommet;
    while (token != NULL)
    {
        if (strcmp(token->tag, tagName) == 0)
            return 1;
        token = token->suivant;
    }
    return 0;
}
void addCategorie(char *Path, char *tagName)
{
    const char *fichier = Path;
    char buff[MAXLEN];
    int size = listxattr(fichier, buff, sizeof(buff));
    Tags *list = malloc(sizeof(Tags));
    list->NbTags = 0;
    list->sommet = NULL;
    if (size > 0)
    {
        ListOfTags(list, buff, size);
    }
    if (findInList(list, tagName))
    {
        printf("Error , category exist");
    }
    else
    { // new categorie
        set_tags(Path, "", tagName, 0);
    }
}
char *TagsToBuf(Tags *tags)
{
    Token *token = tags->sommet;
    char *data = malloc(sizeof(char) * MAXLEN);
    while (token != NULL)
    {
        strcat(token->tag, "#");
        strcat(data, token->tag);
        printf("tag :%s\n", token->tag);
        token = token->suivant;
    }
    printf("\ndata :%s\n", data);
    return data;
}

void addTagInCategorie(char *Path, char *category, char *tagName)
{

    const char *fichier = Path;
    char buff[MAXLEN];
    int size = listxattr(fichier, buff, sizeof(buff));
    Tags *list = malloc(sizeof(Tags));
    list->NbTags = 0;
    list->sommet = NULL;
    if (size > 0)
    {
        ListOfTags(list, buff, size);
    }
    if (findInList(list, category))
    {
        Tags *listOfTags = malloc(sizeof(Tags));
        listOfTags->NbTags = 0;
        listOfTags->sommet = NULL;
        get_tags(listOfTags, Path, category);
        if (findInList(listOfTags, tagName) == 0)
        {
            add(listOfTags, tagName); /// add the tag to the list of tags
            char *buffer;
            buffer = TagsToBuf(listOfTags);
            printf("%s\n", buffer);
            set_tags(Path, buffer, category, 1);
        }
        else
            printf("\ntag exist\n");
    }
    else
    {
        printf("Error , category doesn't exist");
    }
}

void removeTagCategory(char *Path, char *category, char *tagName)
{
    const char *fichier = Path;
    char buff[MAXLEN];
    int size = listxattr(fichier, buff, sizeof(buff));
    Tags *list = malloc(sizeof(Tags));
    list->NbTags = 0;
    list->sommet = NULL;
    if (size > 0)
    {
        ListOfTags(list, buff, size);
    }
    if (findInList(list, category))
    {
        Tags *listOfTags = malloc(sizeof(Tags));
        listOfTags->NbTags = 0;
        listOfTags->sommet = NULL;
        get_tags(listOfTags, Path, category);
        deletTag(listOfTags, tagName);
        char *buffer;
        buffer = TagsToBuf(listOfTags);
        printf("%s\n", buffer);
        set_tags(Path, buffer, category, 1);
    }
    else
    {
        printf("Error , category doesn't exist");
    }
}
void removeTag(char *Path, char *tagName)
{
    const char *fichier = Path;
    char buff[MAXLEN];
    int size = listxattr(fichier, buff, sizeof(buff));
    Tags *list = malloc(sizeof(Tags));
    list->NbTags = 0;
    list->sommet = NULL;
    if (size > 0)
    {
        ListOfTags(list, buff, size);
    }
    Token *tmp = list->sommet;
    int cnt = 0;
    while (tmp != NULL && cnt == 0)
    {
        Tags *listOfTags = malloc(sizeof(Tags));
        listOfTags->NbTags = 0;
        listOfTags->sommet = NULL;
        get_tags(listOfTags, Path, tmp->tag);
        if (findInList(listOfTags, tagName))
        {
            printf("\nremove\n");
            deletTag(listOfTags, tagName);
            char *buffer;
            buffer = TagsToBuf(listOfTags);
            printf("%s\n", buffer);
            set_tags(Path, buffer, tmp->tag, 1);
            cnt = 1;
        }
        else
            tmp = tmp->suivant;
    }
}

void supprimerCategorie(char *Path, char *category)
{
    const char *fichier = Path;
    char buf[30];
    sprintf(buf, "user.%s", category);
    int supr = removexattr(fichier, buf);
    if (supr == 0)
    {
        printf("success ,supprimer Categorie ");
    }
    else
        printf("Error ,supprimer Categorie ");
}
void afficher(Tags *tags)
{

    Token *token = tags->sommet;
    while (token != NULL)
    {
        printf("  - tag :%s\n", token->tag);
        token = token->suivant;
    }
}

void listTag(char *Path)
{
    const char *fichier = Path;
    char buff[MAXLEN];
    int size = listxattr(fichier, buff, sizeof(buff));
    Tags *list = malloc(sizeof(Tags));
    list->NbTags = 0;
    list->sommet = NULL;
    if (size > 0)
    {
        ListOfTags(list, buff, size);
    }
    Token *token = list->sommet;
    if (token == NULL)
    {
        printf("\nList vide \n ");
        return;
    }
    while (token != NULL)
    {
        printf(" Categorie :%s\n", token->tag);
        Tags *listOfTags = malloc(sizeof(Tags));
        listOfTags->NbTags = 0;
        listOfTags->sommet = NULL;
        get_tags(listOfTags, Path, token->tag);
        afficher(listOfTags);
        token = token->suivant;
    }
}
void addListInLIst(Tags *res,Tags *list){
    Token *token=list->sommet;
    while(token!=NULL) {
        add(res,token->tag);
        token=token->suivant;
    }
}
Tags *Allsoustags(char *Path, Tags *listcat){
    Tags *res = malloc(sizeof(Tags));
    res->NbTags = 0;
    res->sommet = NULL;
    Token *tok=listcat->sommet;
    while (tok!=NULL){
        Tags *listOfTags = malloc(sizeof(Tags));
        listOfTags->NbTags = 0;
        listOfTags->sommet = NULL;
        get_tags(listOfTags, Path, tok->tag);
        add(res,tok->tag);
        addListInLIst(res,listOfTags);
        tok=tok->suivant;
    }
    return res;

}

int contientTag(char *Path, char *tag)
{
    const char *fichier = Path;
    char buff[MAXLEN];
    int size = listxattr(fichier, buff, sizeof(buff));
    Tags *listcat = malloc(sizeof(Tags));
    listcat->NbTags = 0;
    listcat->sommet = NULL;
    if (size > 0)
    {
        ListOfTags(listcat, buff, size);
    }
    if (listcat->sommet == NULL) return 0;
    Tags *listall=Allsoustags(Path,listcat);
    return findInList(listall,tag);
}