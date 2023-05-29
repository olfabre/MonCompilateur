# Mon compilateur
Un simple compilateur de langage impératif LL(k) pour Pascal en code Assembleur 64 bit 80x86 (AT&T)

**Important**:  
il y a 3 versions du compilateur:  
La version 3 (compilateur3.zip) est basée sur votre dépôt Compilateur-master pour réaliser le TP n°8.  
La version 2 (compilateur2.zip) est une version codée personnellement du TP3 à TP5 avec Flex++.   
La version 1 (compilateur1.zip) est une version codée personnellement du TP1 à TP2 avec Flex++.  
Ci-dessous toutes les détails:

# Version compilateur3.zip #  

**Travail perso effectué sur cette version :**  
Le travail a été réalisé survotre dépôt master car je n'avais pas terminé tous les TPs.  
Si vous souhaitez voir mon travail avant le TP8, merci de voir la version compilateur2.zip et compilateur1.zip  
La boucle FOR a été réalisé dans la méthode:
- void ForStatement()

J'ai modifié la méthode:

- Expression();

J'ai modifié le fichier "tokeniser.l" pour ajouter des nouveaux keyword.


J'ai inclu la structure de contrôle du CASE selon la grammaire en Pascal:

> https://condor.depaul.edu/ichu/csc447/notes/wk2/pascal.html

Des nouvelles méthodes ont été créées:

- void CaseStatement();
- void CaseListElement();
- void CaseLabelList();

J'ai modifié les méthodes suivantes:
- void Statement(void);
- void StatementPart(void)

J'ai ajouté de nouvelles variables globales:
- unsigned long long tagFor; // Permet d'obtenir un nommage unique des étiquettes de l'instruction FOR
- string keywordCurrently=""; // Permet de mémoriser le témoin d'une instruction en cours
- int compteurCaseLabel; // Permet de savoir le nombre de labels

Explications de ma méthode CASE en Assembleur:

CASE n OF  
------------1,2: j:=5;  
------------3,4: j:=6;  
------------5,6: j:=10  
END

Soit n := 2;
Je compare la valeur de n à chaque label et par exemple pour le premier cycle, je le compare avec la valeur 1.  
Si la valeur n et le label, ne sont pas égaux, je passe à la seconde valeur de la liste.
Ici, n est bien egale à 2 donc j'affecte la constante 5 à j.




**Règles de syntaxe réalisées (EBNF) sur cette vesion :**

Règles de grammaire (Version compilateur3.zip):

> Program := [DeclarationPart] StatementPart  
> DeclarationPart := "[" Letter {"," Letter} "]"  
> StatementPart := Statement {";" Statement} "."  
> Statement := AssignementStatement|DisplayStatement|IfStatement|ForStatement|WhileStatement|BlockStatement  
> AssignementStatement := Letter "=" Expression

> Expression := SimpleExpression [RelationalOperator SimpleExpression]   
> SimpleExpression := Term {AdditiveOperator Term}  
> Term := Factor {MultiplicativeOperator Factor}  
> Factor := Number | Letter | "(" Expression ")"| "!" Factor  
> Number := Digit{Digit}

> AdditiveOperator := "+" | "-" | "||"  
> MultiplicativeOperator := "*" | "/" | "%" | "&&"  
> RelationalOperator := "==" | "!=" | "<" | ">" | "<=" | ">="    
> Digit := "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"  
> Letter := "a"|...|"z"  
> DisplayStatement := "DISPLAY" Expression  
> ForStatement := "For" ID ":=" Expression ("TO"|"DOWNTO") Expression "DO" Statement  
> WhileStatement := "WHILE" Expression "DO" Statement  
> BlockStatement := "BEGIN" Statement {";" Statement} "END"  
> IfStatement := "IF" Expression "THEN" Statement ["ELSE" Statement]

> CaseStatement := "CASE" Expression "OF" CaseListElement { ";" CaseListElement } "END"  
> CaseListElement := CaseLabelList ":" (Statement  | "")  
> CaseLabelList :=  Number {"," Number }



Jeu de tests:

> VAR	a, i :	INTEGER.  
> i := 0;  
> FOR a:=0 TO a<10 DO i:=i+1;  
> FOR i:=10 DOWNTO i<1 DO a:=a+1.  
> CASE n OF  
>       1,2: j:=5;  
>       3,4: j:=6;  
>       5,6: j:=10;  
> END

# Version perso compilateur2.zip #  

**Travail effectué sur cette version :**  
Le point de départ est le fichier compilateur.cpp de ma version compilateur1.  
J'ai modifié le fichier "tokeniser.l" afin d'ajouter de
nouvelles reconnaissances de jeton qui sera utilisée
lors de la création du "tokeniser.cpp" avec le script make:

> instructs (IF|THEN|ELSE|WHILE|DO|FOR|TO|BEGIN|END)  
> {instruct}  return INSTRUCT;

J'ai modifié le fichier "tokeniser.h" afin d'ajouter le nouveau TOKEN:

> enum TOKEN {FEOF, UNKNOWN, NUMBER, ID, STRINGCONST, RBRACKET, LBRACKET, RPARENT, LPARENT, COMMA, SEMICOLON, DOT, ADDOP, MULOP, RELOP, NOT, ASSIGN, INSTRUCTS};

J'ai regeneré un nouveau fichier tokenniser.cpp pour créer le nouveau fichier objet tokenniser.o

Modifications des méthodes suivantes:
- void Statement();
- void Expression();
- void SimpleExpression();
- void AssignementStatement();
- void Number()
- void Factor()

Création des méthodes suivantes:
- void IfStatement();
- void WhileStatement();
- void ForStatement();
- void BlockStatement();



**Règles de syntaxe réalisées (EBNF) sur cette vesion :**

Règles de grammaire (Version compilateur2.zip):

> Program := [DeclarationPart] StatementPart  
> DeclarationPart := "[" Letter {"," Letter} "]"  
> StatementPart := Statement {";" Statement} "."  
> Statement := AssignementStatement | IfStatement | WhileStatement | ForStatement | BlockStatement  
> AssignementStatement := Letter "=" Expression  
> Expression := SimpleExpression [RelationalOperator SimpleExpression]  
> SimpleExpression := Term {AdditiveOperator Term}  
> Term := Factor {MultiplicativeOperator Factor}  
> Factor := Number | Letter | "(" Expression ")"| "!" Factor  
> Number := Digit{Digit}  
> Digit := "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"  
> Letter := "a"|...|"z"  
> IfStatement := "IF" Expression "THEN" Statement [ "ELSE" Statement ]  
> WhileStatement := "WHILE" Expression "DO" Statement  
> ForStatement := "FOR" AssignementStatement "To" Expression "DO" Statement  
> BlockStatement := "BEGIN" Statement { ";" Statement } "END"

Jeu de tests:

> [a,b,c,z]  
z:=(8==3)||(4==2*2);  
IF 5 > 7 THEN c:=5 ELSE c:=102;  
WHILE a<3 DO a:=a+1;  
FOR i:=2 TO i<=10 DO c:=c+1;  
BEGIN  
a:=a+1;  
WHILE a<3 DO a:=a+1;  
END.




# Version perso compilateur1.zip #  

**Travail effectué sur cette version :**  
J'ai pris mon ancien code du TP1&2 et j'ai intégré à l'aide votre dépôt 1.0 (TP3), la  
bibliothèque <FlexLexer.h> pour l'intégration des analyseurs lexicaux générés par Flex  
J'ai commenté chaque ligne pour une meilleur compréhension du code.    
J'ai placé le nommage des méthodes dans la partie PROTOTYPES pour réorganiser le code pour avoir la méthode principale 'main' en début.

Modification et personnalisation des méthodes suivantes:
- void Error(const string& s)
- void DeclarationPart()
- void StatementPart()
- void Statement()
- void AssignementStatement()
- void Expression()
- void SimpleExpression()
- void Term()
- void Factor()

**Règles de syntaxe réalisées (EBNF) sur cette version :**

Règles de grammaire (Version compilateur1.zip):

> Program := [DeclarationPart] StatementPart  
> DeclarationPart := "[" Letter {"," Letter} "]"  
> StatementPart := Statement {";" Statement} "."  
> Statement := AssignementStatement  
> AssignementStatement := Letter "=" Expression  
> Expression := SimpleExpression [RelationalOperator SimpleExpression]  
> SimpleExpression := Term {AdditiveOperator Term}  
> Term := Factor {MultiplicativeOperator Factor}  
> Factor := Number | Letter | "(" Expression ")"| "!" Factor  
> Number := Digit{Digit}
> Digit := "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"  
> Letter := "a"|...|"z"

Jeu de tests:

> [a,b,c,z]  
> z:=(8==3)||(4==2*2);  
> b:=(5/65+2)<(7%5).



**Commandes utiles :**

> make

> g++ -ggdb -o compilateur compilateur.cpp tokeniser.o

> ./compilateur <test.p >test.s


