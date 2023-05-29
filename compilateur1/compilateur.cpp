///     ___  _ _       _             _____     _
///    / _ \| (_)_   _(_) ___ _ __  |  ___|_ _| |__  _ __ ___
///   | | | | | \ \ / / |/ _ \ '__| | |_ / _` | '_ \| '__/ _ \
///   | |_| | | |\ V /| |  __/ |    |  _| (_| | |_) | | |  __/
///    \___/|_|_| \_/ |_|\___|_|    |_|  \__,_|_.__/|_|  \___|
///
///                  https://okgo.direct/website6
/// Réalisation d'un compilateur en C/C++ qui traduira un programme écrit
/// dans un langage pascal simplifié en assembleur 80x86 64 bits
///
///
//  LICENCE ET PATERNITÉ
//  A compiler from a very simple Pascal-like structured language LL(k)
//  to 64-bit 80x86 Assembly langage
//  Copyright (C) 2019 Pierre Jourlin
//
//  This program imakes free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <string> // Utilisation de chaînes de caractères.https://cplusplus.com/reference/cstring/strcmp/?kw=strcmp
#include <iostream>
#include <cstdlib> // fonctionnalités liées à la manipulation des chaînes de caractères (convertion par exemple) https://cplusplus.com/reference/cstdlib/
#include <stack> // Pile LIFO context (last-in first-out)
#include <FlexLexer.h>  // fournit des classes et des fonctionnalités supplémentaires pour aider à l'intégration des analyseurs lexicaux générés par Flex
#include "tokeniser.h" // fournit les définitions des tokens
#include <cstring> // fournit des fonctionnalités pour manipuler des chaînes de caractères (strcmp() entre autre) https://cplusplus.com/reference/cstring/?kw=cstring

using namespace std;


// PROTOTYPES

// Gestion des erreurs
void Error(const string& s);

// Régles de grammaire
void Program();
void DeclarationPart();
void StatementPart();
void Statement();
void AssignementStatement();

void Expression();
void SimpleExpression();
void Term();
void Factor();
void Number();



// Variables globales

stack<string> maPileString; // Pile
int factorNotState = false; // Présence d'un NOT
string variableAssignation; // Variable d'assignation
int compteurEtiquette = 0; // Nommage unique des étiquettes
string variableRELOP; // Mémorisation de la relation en cours
string variableADDOP; // Mémorisation de l'opération en cours
string variableMULOP; // Mémorisation de l'opération en cours



TOKEN current;	// Creation d'une variable de type TOKEN pour stocker le jeton courant lors de l'analyse lexicale
FlexLexer* lexer = new yyFlexLexer;  // lexer sera une instance de la classe yyFlexLexer qui est s'occupe de la reconnaissance et de la génération des jetons
// lexer->yylex() renvoie le type de l'entrée lexicale  (définie dans enumération TOKEN ci-dessus)
// lexer->YYText() renvoie l'entrée lexicale sous forme de chaîne de caractères (Attention: pointe sur le premier indice du tableau de caractères char*)




// Méthode principale -> point d'entrée
int main() {
    // En-tête pour le compilateur gcc et l'éditeur des liens (\t saut chariot)
    cout << "#Ce code a été produit par mon compilateur dans le cadre de l'UE Assembleur et Compilation." << endl;
    cout << "#Enseignant: Pierre Jourlin, Université d'Avignon." << endl;

    current = (TOKEN)lexer->yylex(); // retourne la valeur de retour de la fonction lexer->yylex() qui est convertie en type "TOKEN"

    Program(); // Début du programme d'analyse et production du code des expressions en Pascal

    // Pied de page pour le compilateur gcc et l'éditeur des liens (\t saut chariot)
    // Assembleur x86: Restaurer la position du sommet de la pile
    cout << "\tmovq %rbp, %rsp" << "\t\t\t\t # Main | %rbp -> topPile (Restaurer)" << endl;
    // Assembleur x86: Retour à l'étiquette main
    cout << "\tret" << "\t\t\t\t # Main | saut à l'étiquette main" << endl;

    // Vérification des caractères en trop à la fin du programme
    if (current != FEOF) {
        cerr << "Caractères en trop à la fin du programme : [" << current << "]";
        Error(".");
    }

    return 0; // Indique une exécution réussie du programme
}

void Error(const string& s) {
    // lexer->lineno() retourne le numéro de ligne du fichier lu
    cerr << "Ligne n°" << lexer->lineno() << ", lu : '" << lexer->YYText() << "'(" << current << "), mais "
         << s << endl;
    exit(-1);
}

void Program() {



    // Règle optionnelle: DeclarationPart
    if (current == RBRACKET) {
        DeclarationPart();
    }

    // Règle obligatoire: StatementPart
    StatementPart();

}


// La règle DeclarationPart est définie comme "[" Letter {"," Letter} "]"
void DeclarationPart() {

    // Régle d'entrée obligatoire: le premier jeton doit être obligatoirement RBRACKET "["
    if (current == RBRACKET) {


            // Assembleur x86: section .data est utilisée pour allouer et initialiser des variables globales et des constantes
            cout << "\t.data" << "\t\t\t\t # DeclarationPart" << endl;


            // Lecture du jeton suivant
            current = (TOKEN) lexer->yylex();

            // Régle obligatoire: présence d'au moins 1 lettre ID ou plusieurs séparée par une virgule COMMA
            if (current == ID) {

                        // Assembleur x86: réserver un emplacement en mémoire de 64 bits (8 octets) et l'initialiser avec la valeur zéro pour une variable
                        cout << lexer->YYText() << ":\t.quad 0" << "\t\t\t\t # DeclarationPart" << endl;

                        // On empile le nom de la variable our l'utiliser par la suite
                        maPileString.push(lexer->YYText());


                        // Lecture du jeton suivant
                        current = (TOKEN) lexer->yylex();

                                // Régle Optionnelle: présence de plusieurs lettres séparées par une virgule ?
                                // Si oui tant que le jeton actuelle est une virgule COMMA
                                while (true){

                                    // Lecture du jeton suivant
                                    current = (TOKEN) lexer->yylex();

                                        // Régle obligatoire: présence d'une lettre ID
                                        if (current == ID){

                                            // Assembleur x86: réserver un emplacement en mémoire de 64 bits (8 octets) et l'initialiser avec la valeur zéro pour une variable
                                            cout << lexer->YYText() << ":\t.quad 0" << "\t\t\t\t # DeclarationPart" << endl;

                                            // On empile le nom de la variable our l'utiliser par la suite
                                            maPileString.push(lexer->YYText());


                                            // Lecture du jeton suivant
                                            current = (TOKEN) lexer->yylex();

                                            // Et on boucle tant que le jeton actuelle est une virgule

                                        }else{Error("Un ID était attendu");}

                                        // Aucune virgule
                                        if (current != COMMA) {
                                            // On sort de la boucle while
                                            break;
                                        }
                                }

            }
            else{Error("Un ID était attendu");}


    } // Sinon on génère une erreur
    else { Error("Un RBRACKET était attendu"); }


    // Règle de sortie obligatoire: le dernier jeton doit être obligatoirement LBRACKET "]"
    if (current == LBRACKET) {
            // Lecture du jeton suivant
            current = (TOKEN) lexer->yylex();
    }else{ Error("Un LBRACKET était attendu");}

}



// La règle StatementPart est définie comme Statement {";" Statement} "."
void StatementPart() {

    // Partie des instructions

    // Assembleur x86: Début du programme .text
    cout << "\t.text" << "\t\t\t\t # StatementPart" << endl;
    // Assembleur x86: la fonction main est maintenant visible de l'exterieur .globl
    cout << "\t.globl main" << "\t\t\t\t # StatementPart" << endl;
    // Assembleur x86: la fonction main point d'entrée
    cout << "main:" << "\t\t\t\t # StatementPart" << endl;
    // Assembleur x86: sauvegarder la position du sommet de la pile
    cout << "\tmovq %rsp, %rbp" << "\t\t\t\t # StatementPart | topPile -> %rbp (sauvegarder)" << endl;

    // Régle obligatoire: régle définie dans Statement()
    Statement();

    // Régle optionnelle: présence 0 ou 1 ou plusieurs Statement précédé(s) obligatoirement par un SEMICOLON
    while (current == SEMICOLON) {
        // Lecture du jeton suivant
        current = (TOKEN) lexer->yylex();
        // Régle obligatoire: régle définie dans Statement()
        Statement();
    }

    // Régle obligatoire de sortie: présence obligatoire d'un DOT en fin du groupe d'instructions
    if (current == DOT){

        // Fin du groupe d'instructions, on peut quitter la méthode
        // Lecture du jeton suivant
        current = (TOKEN) lexer->yylex();
    }else{Error("Un DOT était attendu");}


}



// La règle Statement est définie comme AssignementStatement
void Statement(){

    // Régle obligatoire: régle définie dans AssignementStatement()
    AssignementStatement();
}




// La règle AssignementStatement est définie comme Letter ":=" Expression
void AssignementStatement(){


    // Régle d'entrée obligatoire: le premier jeton doit être obligatoirement ID
    if(current == ID){

        // On mémorise la variable qui sera assignée à la fin par retour
        variableAssignation = lexer->YYText();

        // Lecture du jeton suivant
        current=(TOKEN) lexer->yylex();


                    // Régle obligatoire: présence d'une assignation ASSIGN ":="
                    if(current == ASSIGN) {

                        // Lecture du jeton suivant
                        current = (TOKEN) lexer->yylex();

                        // Régle obligatoire: régle définie dans Expression()
                        Expression();

                        // Assembleur x86: on récupère la valeur dans la pile pour l'assigner à la variable
                        cout << "\tpop " << variableAssignation << "\t\t\t\t # AssignementStatement | topPile -> " << variableAssignation << endl;
                    }else{ Error("Un ASSIGN était attendu");}

    }else{Error("Un ID était attendu");}


}


// La règle Expression est définie comme SimpleExpression [RelationalOperator SimpleExpression]
void Expression(){


    // Régle obligatoire: régle définie dans SimpleExpression()
    SimpleExpression();


    // Règle optionnelle (O ou 1 fois): le jeton doit être un RELOP  "==" ou "!=" ou "<" ou ">" ou "<=" ou ">="
    if(current == RELOP){
        //cout << lexer->YYText();

        // YYText() ne renvoie pas une chaîne de caractère de type string, mais un char *
        // On va donc utiliser la fonction strcmp() qui renvoie 0, donc False, quand les deux chaînes sont identiques.
        if(!strcmp(lexer->YYText(),"==")) {
            //Assembleur x86: saut si la condition suivante est valide:  %rax = 0
            variableRELOP = "\tje siVrai" + to_string(compteurEtiquette) + "\t\t\t\t # Expression | si %rax = 0 alors siVrai" + to_string(compteurEtiquette);
        }
        if(!strcmp(lexer->YYText(),"!=")){
            //Assembleur x86: saut si la condition suivante est valide:  %rax != 0
            variableRELOP = "\tjne siVrai" + to_string(compteurEtiquette) + "\t\t\t\t # Expression | si %rax != 0 alors siVrai" + to_string(compteurEtiquette);
        }
        if(!strcmp(lexer->YYText(),"<")){
            //Assembleur x86: saut si la condition suivante est valide:  %rax < 0
            variableRELOP = "\tjb siVrai" + to_string(compteurEtiquette) + "\t\t\t\t # Expression | si %rax < 0 alors siVrai" + to_string(compteurEtiquette);
        }
        if(!strcmp(lexer->YYText(),">")){
            //Assembleur x86: saut si la condition suivante est valide:  %rax > 0
            variableRELOP = "\tja siVrai" + to_string(compteurEtiquette) + "\t\t\t\t # Expression | si %rax > 0 alors siVrai" + to_string(compteurEtiquette);
        }
        if(!strcmp(lexer->YYText(),"<=")){
            //Assembleur x86: saut si la condition suivante est valide:  %rax <= 0
            variableRELOP = "\tjbe siVrai" + to_string(compteurEtiquette) + "\t\t\t\t # Expression | si %rax <= 0 alors siVrai" + to_string(compteurEtiquette);
        }
        if(!strcmp(lexer->YYText(),">=")){
            //Assembleur x86: saut si la condition suivante est valide:  %rax >= 0
            variableRELOP = "\tjae siVrai" + to_string(compteurEtiquette) + "\t\t\t\t # Expression | si %rax >= 0 alors siVrai" + to_string(compteurEtiquette);
        }

        // Lecture du jeton suivant
        current = (TOKEN) lexer->yylex();

        // Régle obligatoire: régle définie dans SimpleExpression()
        SimpleExpression();

        //Assembleur x86: on récupère la valeur dans le top de la pile pour la mettre dans le registre %rax
        cout << "\tpop %rax" << "\t\t\t\t # Expression | topPile -> %rax" << endl;
        //Assembleur x86: on récupère la valeur dans le top de la pile pour la mettre dans le registre %rbx
        cout << "\tpop %rbx" << "\t\t\t\t # Expression | topPile -> %rbx" << endl;
        //Assembleur x86: on compare (sub) %rax avec %rbx
        cout << "\tcmpq %rax, %rbx" << "\t\t\t\t # Expression | %rax - %rbx -> %rax (comparer)" << endl;


        // Affichage de la relation mémorisée
        cout << variableRELOP << endl;

        // traitement des cas dans un ordre bien spécifique (d'abord le faux ensuite le vrai et pour finir l'étiquette pour continuer le code séquentielle)

        //part 1
        //Assembleur x86: dans le cas faux, on empile la valeur Min à la pile
        cout << "\tpush $0" << "\t\t\t\t # Expression | cas faux (valeur Min)" << endl;
        //Assembleur x86: on saute directement à l'étiquette nommée
        cout << "\tjmp siNon" << compteurEtiquette << "\t\t\t\t # Expression | cas faux: on saute l'étiquette siNon"  << compteurEtiquette << endl;
        //part 2
        //Assembleur x86: création étiquette unique pour le cas vrai
        cout << "siVrai" << compteurEtiquette << ":" << endl;
        //Assembleur x86: dans le cas vrai, on empile la valeur Max à la pile
        cout << "\tpush $0xFFFFFFFFFFFFFFFF" << "\t\t\t\t # Expression | cas vrai (valeur Max)" << endl;
        //part 3
        //Assembleur x86: création étiquette unique pour le cas faux
        cout << "siNon" << compteurEtiquette << ":" << endl;

        // Incrémente pour avoir un nommage unique
        compteurEtiquette++;

        // Initialise
        variableRELOP="";

    }


}

// La règle SimpleExpression est définie comme Term {AdditiveOperator Term}
void SimpleExpression(){



    // Régle obligatoire: régle définie dans Term()
    Term();

    // Règle optionnelle (0 ou 1 ou +): le jeton doit être un ADDOP  "+" ou "-" ou "||"
    while(current == ADDOP){

        // YYText() ne renvoie pas une chaîne de caractère de type string, mais un char *
        // On va donc utiliser la fonction strcmp() qui renvoie 0, donc False, quand les deux chaînes sont identiques.
        if(!strcmp(lexer->YYText(),"+")){
            //Assembleur x86: ajoute %rbx à %rax
            variableADDOP =  "\taddq	%rbx, %rax \t\t\t\t # SimpleExpression | %rax + %rbx -> %rax\n";
            //Assembleur x86: empile %rax à la pile
            variableADDOP += "\tpush %rax \t\t\t\t # SimpleExpression | %rax -> topPile (Stockage résultat ADD)\n";
        }
        if(!strcmp(lexer->YYText(),"-")){
            //Assembleur x86: ajoute -(%rbx) à %rax
            variableADDOP =  "\tsubq	%rbx, %rax \t\t\t\t # SimpleExpression | %rax - %rbx -> %rax\n";
            //Assembleur x86: empile %rax à la pile
            variableADDOP += "\tpush %rax \t\t\t\t # SimpleExpression | %rax -> topPile (Stockage résultat SUB)\n";
        }
        if(!strcmp(lexer->YYText(),"||")){
            //Assembleur x86: ajoute %rbx à %rax
            variableADDOP =  "\taddq	%rbx, %rax \t\t\t\t # SimpleExpression | %rax + %rbx -> %rax\n";
            //Assembleur x86: empile %rax à la pile
            variableADDOP += "\tpush %rax \t\t\t\t # SimpleExpression | %rax -> topPile (Stockage résultat OR)\n";
        }

        // Lecture du jeton suivant
        current=(TOKEN) lexer->yylex();

        // Régle obligatoire: régle définie dans Term()
        Term();

        //Assembleur x86: depile %rbx de la pile
        cout << "\tpop %rbx" << "\t\t\t\t # SimpleExpression | topPile -> %rbx" << endl;
        //Assembleur x86: depile %rax de la pile
        cout << "\tpop %rax"<< "\t\t\t\t # SimpleExpression | topPile -> %rax" << endl;


        // Affichage de l'opération mémorisée
        cout << variableADDOP;

        // Initialise
        variableADDOP="";


    }

}

// La régle Term est définie comme Factor {MultiplicativeOperator Factor}
void Term(){


    //"Term" est une autre règle de la grammaire qui définit un terme dans une expression arithmétique.


    // Régle obligatoire: régle définie dans Factor()
    Factor();

    // Règle optionnelle (0 ou 1 ou +): le jeton doit être un MULOP  "*" ou "/" ou "%" ou "&&"
    while(current == MULOP){

        // YYText() ne renvoie pas une chaîne de caractère de type string, mais un char *
        // On va donc utiliser la fonction strcmp() qui renvoie 0, donc False, quand les deux chaînes sont identiques.

        // MUL
        if(!strcmp(lexer->YYText(),"*")){
            //Assembleur x86: on multiplie %rbx par %rax
            variableMULOP = "\tmulq	%rbx \t\t\t\t # Term | %rbx * %rax -> %rax\n";
            //Assembleur x86: on stocke le résultat en empilant %rax dans la pile
            variableMULOP += "\tpush %rax \t\t\t\t # Term |  %rax -> topPile (Stockage résultat MUL)\n";
        }
        // DIV
        if(!strcmp(lexer->YYText(),"/")){
            //Assembleur x86: initialisé à zéro %rdx (le reste)
            variableMULOP = "\tmovq $0, %rdx \t\t\t\t # Term |  0 -> %rdx\n";
            //Assembleur x86: on divise %rax par %rbx et %rax récupérera seulement le quotient
            variableMULOP += "\tdiv %rbx \t\t\t\t # Term | %rax / %rbx -> %rax (quotient)\n";
            //Assembleur x86: on stocke le résultat du quotient en empilant %rax dans la pile
            variableMULOP += "\tpush %rax \t\t\t\t # Term |  %rax -> topPile (Stockage résultat DIV)\n";
        }
        // MODULO
        if(!strcmp(lexer->YYText(),"%")){
            //Assembleur x86: initialisé à zéro %rdx (le reste)
            variableMULOP = "\tmovq $0, %rdx \t\t\t\t # Term |  0 -> %rdx\n";
            //Assembleur x86: on divise %rax par %rbx et %rdx récupérera seulement le reste
            variableMULOP += "\tdiv %rbx \t\t\t\t # Term | %rax / %rbx -> %rdx (reste)\n";
            //Assembleur x86: on stocke le résultat du reste en empilant %rdx dans la pile
            variableMULOP += "\tpush %rdx\t# MOD \t\t\t\t # Term | %rdx -> topPile (Stockage résultat MOD)\n";
        }
        // AND
        if(!strcmp(lexer->YYText(),"&&")){
            //Assembleur x86: on multiplie %rbx par %rax (même logique 1 et 1 = 1 * 1 = 1 | 1 et 0 = 1 * 0 = 0)
            variableMULOP = "\tmulq	%rbx \t\t\t\t # Term | %rbx * %rax -> %rax\n";
            //Assembleur x86: on stocke le résultat en empilant %rax dans la pile
            variableMULOP += "\tpush %rax \t\t\t\t # Term |  %rax -> topPile (Stockage résultat AND)\n";
        }

        // Lecture du jeton suivant
        current=(TOKEN) lexer->yylex();

        // Régle obligatoire: régle définie dans Factor()
        Factor();

        //Assembleur x86: depile %rbx de la pile
        cout << "\tpop %rbx" << "\t\t\t\t # Term | topPile -> %rbx" << endl;
        //Assembleur x86: depile %rax de la pile
        cout << "\tpop %rax" << "\t\t\t\t # Term | topPile -> %rax" << endl;

        // Affichage de l'opération mémorisée
        cout << variableMULOP;

        // Initialise
        variableMULOP="";
    }
}

// La régle Factor est définie comme Number | Letter | "(" Expression ")" | "!" Factor
void Factor(){


    // On attend soit un nombre, soit une lettre, soit une acollade droite suivi d'une expression, soit un point d'exclamation suivi d'un autre Factor
    switch(current){

        case NUMBER:

            Number();
            break;
        case ID:

            // Assembleur x86: le contenu de la variable est stockée sur la pile et le pointeur de pile (%rsp) est ajusté pour pointer vers la nouvelle position en haut de la pile.
            cout << "\tpush " << lexer->YYText() << "\t\t\t\t # Factor | " <<  lexer->YYText() << " -> topPile"  << endl;

            // Lecture du jeton suivant
            current=(TOKEN) lexer->yylex();
            break;

        case RPARENT:

            // Lecture du jeton suivant
            current=(TOKEN) lexer->yylex();

            // Règle obligatoire: régle définie dans Expression()
            Expression();

            // Règle obligatoire de sortie: présence d'une acollade gauche LPARENT
            if(current == LPARENT){

                // Lecture du jeton suivant
                current=(TOKEN) lexer->yylex();
            }else{ Error("Un LPARENT était attendu");}
            break;

        case NOT:

            // Présence d'un jeton NOT
            factorNotState = true;

            // Lecture du jeton suivant
            current=(TOKEN) lexer->yylex();

            // Règle obligatoire de sortie: régle définie dans Factor()
            Factor();
            break;

        default:
            Error("un NUMBER ou ID ou RPARENT ou NOT était attendu");
    }



}


void Number(){

    // Assembleur x86: la valeur lue est stockée sur la pile et le pointeur de pile (%rsp) est ajusté pour pointer vers la nouvelle position en haut de la pile.
    cout << "\tpush $" << lexer->YYText() << "\t\t\t\t # Number | " << lexer->YYText() << " -> topPile" << endl;

    // Lecture du jeton suivant
    current=(TOKEN) lexer->yylex();

}





