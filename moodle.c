#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Programadores:
   Douglas Moreira da Silva 233606
   Victor Akio Uchiyama 244974
   Wallace Benedito da Silva 245752
*/


int opcao;
void Login();
int ConsultaD();
void Notas(int ra);
void Matricula(int RA);
int Semestre(int ra);
int Creditos(int ra,int sem_atual);
int credito_disciplina(char disciplina[8]);
int Requisitos(int ra,char disciplina[9],int sem_atual);
void ConsultaHistorico(int ra);


void limparbuffer(void){
	char c;
	while ((c = getchar()) != '\n' && c!= EOF);
}

typedef struct Disciplina{
    char codigo[8];
    char *nome;
    int creditos;
    char *Prerequisito;    
}Disciplina;


typedef struct Aluno{
    int ra;
    char nome[256];
    char login[32];
    char senha[16];
}Aluno;

typedef struct matricula{
    int ra;
    char CodigDisciplina[8];
    int semestre;
    float nota;
    float faltas;

}matricula;

typedef struct Historico{

	int ra;
	char nome[256];
	float CR;
	float notaTotal;
	float creditoTotal;
}CR_alunos;

typedef struct vetCR{

	CR_alunos aluno[1000];
	int top;
}CR_turma;


void ConsultaHistorico(int ra){

	char RA[25]; sprintf(RA, "%i", ra);// sprint faz o oposto do ATOI

	int cont_ra = strlen(RA);
	char  login[32], senha[16];

	int ra_txt, semestre;
	char codigo[8];
	float nota, falta;

	CR_turma consultaH;
	consultaH.top = 0;

	RA[cont_ra]= '.';
	RA[cont_ra+1]= 't';
	RA[cont_ra+2]= 'x';
	RA[cont_ra+3]= 't';
	RA[cont_ra+4]= '\0';			

	//"2, 4, 5, 7, 5, 2, \0"
	//printf("RA:%s\n",RA);



	FILE * fp[2];

	fp[0] = fopen("Alunos.txt","r");
	
	while(fscanf(fp[0],"%d,%[^,],%[^,],%[^\n]\n",&consultaH.aluno[consultaH.top].ra,consultaH.aluno[consultaH.top].nome,login,senha) != EOF){
			consultaH.aluno[consultaH.top].notaTotal = 0;
			consultaH.aluno[consultaH.top].creditoTotal = 0;
			consultaH.top++;
	}fclose(fp[0]);

	for(int c = 0;c < consultaH.top;c++){
		///rewind
		fp[0] = fopen("AlunosDisciplinas.txt","r");
		while(fscanf(fp[0],"%d;%[^;];%d;%f;%f\n",&ra_txt,codigo,&semestre,&nota,&falta) != EOF){
			if(ra_txt == consultaH.aluno[c].ra){
				int credito_da_disciplina = credito_disciplina(codigo);
				consultaH.aluno[c].notaTotal += credito_da_disciplina*nota;
				consultaH.aluno[c].creditoTotal += credito_da_disciplina;
			}
		}fclose(fp[0]);		
		consultaH.aluno[c].CR = consultaH.aluno[c].notaTotal /consultaH.aluno[c].creditoTotal;		
	}

	//printf("1 2 3\n\n");

	//fp[0] =
	FILE * F = fopen(RA,"w");
	int classificacao;

	for(int c = 0;c < consultaH.top;c++){
		if(consultaH.aluno[c].ra == ra){
			classificacao = 1;
			for(int j = 0;j < consultaH.top;j++){
				if(consultaH.aluno[j].CR > consultaH.aluno[c].CR)
					classificacao++;
			}

			fprintf(F,"Faculdade de Tecnologia –UNICAMP\n\n");
			fprintf(F,"Relatório de Matrícula\n\n");
			fprintf(F,"Nome Completo: %s\n",consultaH.aluno[c].nome);
			fprintf(F,"RA: %d\n",ra);
			fprintf(F,"Coeficiente de Rendimento %.1f\n",consultaH.aluno[c].CR);
			fprintf(F,"Classificação do aluno na turma : %d de %d\n\n",classificacao,consultaH.top);			
		}
	}

	fp[1] = fopen("AlunosDisciplinas.txt","r");
		
	fprintf(F,"Disciplina\tNota\tFaltas(%%)\tSituação\n");

	while(fscanf(fp[1],"%d;%[^;];%d;%f;%f\n",&ra_txt,codigo,&semestre,&nota,&falta) != EOF){
		if(ra_txt == ra){
			if(nota >=5){
				if(falta <= 25){
					fprintf(F,"%s\t\t%.1f\t%.1f\t\tAprovado por Nota e Frequência\n",codigo,nota,falta);
				}else{
					fprintf(F,"%s\t\t%.1f\t%.1f\t\tReprovado por Faltas\n",codigo,nota,falta);
				}
			}else
				if(falta > 25){
					fprintf(F,"%s\t\t%.1f\t%.1f\t\tReprovado por Nota e Frequência\n",codigo,nota,falta);
				}else{
					fprintf(F,"%s\t\t%.1f\t%.1f\t\tReprovado por Nota\n",codigo,nota,falta);
				}
		}
	}fclose(F);
	fclose(fp[1]);

	printf("Arquivo (%s) gerado com sucesso\n",RA);
}

int cont_alunos(){

	int cont = 0;

	int Ra,semestre,sem_txt;
	char disciplina[8];
	float nota, falta;

	FILE *fp = fopen("AlunosDisciplinas.txt","r");
	if (fp == NULL){
		perror("Erro: função cont alunos FILE");
	}
	while(fscanf(fp,"%d;%[^;];%d;%f;%f",&Ra,disciplina,&sem_txt,&nota,&falta)!=EOF){
		cont++;
	}

	return cont;
	fclose(fp);
}


void Notas(int ra){


	int Ra,semestre,sem_txt;
	char disciplina[8];
	float nota, falta;
	int count_alunos, credito_txt;
	char nome[60],codigo[8];
	int Nerror = 15;
	FILE *fp[2];

	do{
		printf("Menu de Alteração de notas e faltas: \n");
		printf("Digite o semestre:\n");
		scanf("%d",&semestre);
		limparbuffer();
		sem_txt = Semestre(ra);
		if(semestre > sem_txt || semestre == 0){
			printf("voce nao esta cursando nenhuma disciplina neste semestre\n\n\nTente novamente\n\n\n\n");
		}
	}while(semestre > sem_txt|| semestre == 0);
	

	fp[0] = fopen("AlunosDisciplinas.txt","r");

	if (fp[0] == NULL){
 		perror("Erro: Function notas FILE ALUNOS DISIPLINA");
	}
	printf("Disciplinas:\n");

	while(fscanf(fp[0],"%d;%[^;];%d;%f;%f",&Ra,disciplina,&sem_txt,&nota,&falta)!=EOF){
		if(Ra == ra && sem_txt == semestre){
			fp[1] = fopen("Disciplinas.txt","r");
			if (fp[1] == NULL){
 				perror("Erro: funcao Notas FILE Dentro do WHILE");
			}
			while(fscanf(fp[1],"%[^;];%[^;];%d\n",codigo,nome,&credito_txt)!= EOF){
				if(strcmp(disciplina, codigo)== 0)
					printf("%s-%s -Nota: %.1f-, Falta(%%): %.1f\n\n",codigo,nome,nota,falta);
			}fclose(fp[1]);
		}
	}

	fclose(fp[0]);

	int i = cont_alunos();
	matricula alunos[i];
	

	i = 0;
	fp[0] = fopen("AlunosDisciplinas.txt","r");
	if (fp[0] == NULL){
		perror("Erro: NOTAS SEgunda abertura ALUNOS_D");
	}
	while(fscanf(fp[0],"%d;%[^;];%d;%f;%f\n",&Ra,disciplina,&sem_txt,&nota,&falta)!=EOF){
		alunos[i].ra = Ra;
		strcpy(alunos[i].CodigDisciplina,disciplina);
    	alunos[i].semestre = sem_txt;
    	alunos[i].nota = nota;
    	alunos[i].faltas = falta;

		i++;
	}
	fclose(fp[0]);

	printf("Digite o codigo da disciplina que deseja fazer alteração:\n");
	scanf("%s",codigo);
	limparbuffer();
	if(strcmp(codigo,"XX000") == 0)
		Nerror = 0;
	else
	for(int c = 0;c < i;c++){
		if(alunos[c].ra == ra && alunos[c].semestre == semestre && strcmp(codigo,alunos[c].CodigDisciplina)== 0){
			do{
				printf("Inserir nova nota:\n");
				scanf("%f",&nota);
				limparbuffer();
				printf("Inserir nova falta:\n");
				scanf("%f",&falta);
				limparbuffer();		
				if(nota >=0.0 && nota <=10.0){
					alunos[c].nota = nota;		
					if(falta >=0.0 && falta <=100.0){
						alunos[c].faltas = falta;
						printf("Atualização realizada com sucesso\n");
						Nerror = 1;
						break;
					}else{
						printf("Valor de nota invalido ou Valor de falta invalido!!\nInsira um valor de nota entre 0 e 10\nInsira um valor de falta entre 0 e 100\n");	
				}
				}else{
						printf("Valor de nota invalido ou Valor de falta invalido!!\nInsira um valor de nota entre 0 e 10\nInsira um valor de falta entre 0 e 100\n");	
				}			
			}while(nota <0 || nota >10 || falta <0 || falta >100);
		}
	}
	if( Nerror != 1 && Nerror != 0)
		printf("Disciplina não encontrada ou inexistente");
	else{
		if(Nerror == 1){
			fp[0] = fopen("AlunosDisciplinas.txt","w");
			if (fp[0] == NULL){
				perror("Erro: NOTAS TERceira ABERTURA ALUNOS _ D");
			}
			for(int c = 0;c < i;c++){
				fprintf(fp[0],"%d;%s;%d;%.1f;%.1f\n",alunos[c].ra,alunos[c].CodigDisciplina,
							alunos[c].semestre,alunos[c].nota,alunos[c].faltas);
				}
			fclose(fp[0]);
		}
	}
}
int Semestre(int ra){
	//printf("Erro SEMESTRe ra %d!\n",ra);
	int Ra,semestre = 0,sem;
	char disciplina[8];
	float nota, falta;

	FILE *fp = fopen("AlunosDisciplinas.txt", "r");
	if (fp == NULL){
 		perror("Erro: SEMESTRE FILE ALUNO_D"); //a mensagem de erro vai ser impressa no terminal
	}


	while(fscanf(fp,"%d;%[^;];%d;%f;%f",&Ra,disciplina,&sem,&nota,&falta)!=EOF){
		//printf("%d;%s;%d;%.1f;%.1f\n\n",Ra,disciplina,sem,nota,falta);
		if( ra == Ra && sem>semestre)
			semestre = sem;
	}
	fclose(fp);

	return semestre;
}

int Creditos(int ra,int sem_atual){

	//printf("Erro Creditos ra %d!  sem atual %d!\n",ra,sem_atual);

	int creditos = 0;
	int ra_txt;
	char materia[8];
	float nota,falta;
	int semestre;

	char codigo[8];
	char nome[60];
	int credito_txt;
	FILE * fp[2];
	
	fp[0] = fopen("AlunosDisciplinas.txt","r");
		if (fp[0] == NULL){
	 		perror("Erro: CREDITOS ALUNO_D "); //a mensagem de erro vai ser impressa no terminal
		}

	while(fscanf(fp[0],"%d;%[^;];%d;%f;%f\n",&ra_txt,materia,&semestre,&nota,&falta)!= EOF){
		//printf("AlunosDisciplinas:%d;%s;%d;%.1f;%.1f\n\n",ra_txt,materia,semestre,nota,falta);
		if(ra_txt == ra && semestre == sem_atual){
			fp[1] = fopen("Disciplinas.txt","r");
			if (fp[1] == NULL){
		 		perror("Erro: CREDITOS DISCIPLINA");
			}
			while(fscanf(fp[1],"%[^;];%[^;];%d\n",codigo,nome,&credito_txt)!= EOF){
				//printf("Disciplinas %s;%s;%d\n\n",codigo,nome,credito_txt);
				if(strcmp(codigo,materia) == 0){
					creditos += credito_txt;
					break;
				}
			}fclose(fp[1]);
		}
	}
	fclose(fp[0]);
	return creditos;
}
int credito_disciplina(char disciplina[9]){
	//printf("Erro Creditos Dissciplina  %s!\n",disciplina);
	
	char codigo[8];
	char nome[60];
	int credito;

	FILE  * fp = fopen("Disciplinas.txt","r");

	while(fscanf(fp,"%[^;];%[^;];%d\n",codigo,nome,&credito)!= EOF){
	//printf("Erro Creditos Dissciplina  %s!\n",codigo);
		if(strcmp(codigo,disciplina) == 0){
			return credito;
		}
	}
	fclose(fp);
	return 0;
}

int Requisitos(int ra,char disciplina[9],int sem_atual){
	//printf("Erro Requisitos ra %d!  sem atual %s!\n",ra,disciplina);

	char semnada[] = "sem_nada";
	char codigo[8], prerequisitos[2][9], txt;
	int ra_txt,semestre;
	char materia[8];
	float nota = 0, falta = 100;
	int conclusao = 0;// 0 nao atende aos requisitos, 1 atende aos requisitos, 2 ja foi aprovado na disciplina cadastrando


	strcpy(prerequisitos[1],"sem_nada");
	//printf("%s\n",prerequisitos[1]);

	FILE * fp;
	fp = fopen("Prerequisitos.txt","r");

	while(fscanf(fp,"%[^;];%s",codigo,prerequisitos[0])!= EOF){
		if(strcmp(codigo,disciplina) == 0){
			//printf("Prerequisitos{ 1!} !!%s!!\n\n",prerequisitos[0]);
			fscanf(fp,"%c",&txt);
			if(txt == ' '){
				fscanf(fp,"%s\n",prerequisitos[1]);
				//printf("Prerequisitos{ 1!} !!%s!!\n\n",prerequisitos[1]);
			}break;
		}fscanf(fp,"\n");
	}
	fclose(fp);
	if(strcmp(codigo,disciplina) != 0){
		conclusao = 3;
		return conclusao;
	}


	fp = fopen("AlunosDisciplinas.txt","r");

	while(fscanf(fp,"%d;%[^;];%d;%f;%f\n",&ra_txt,materia,&semestre,&nota,&falta)!= EOF){
		//printf("ALUnodisciplia.xtx: !!%d;%s;%d;%.1f;%.1f!!!\n",ra_txt,materia,semestre,nota,falta);
		if(ra == ra_txt){
			if(strcmp(materia,disciplina) == 0){
				if(nota >=5.0 && falta <25.0 || semestre == sem_atual){
					conclusao = 2;
					return conclusao;
				}
			}
			if(strcmp(materia,prerequisitos[0]) == 0){
				if(nota >=5.0 && falta <25.0){
					conclusao = 1;
				}else{
					conclusao = 0;
				}
			}
			if(strcmp(materia,prerequisitos[1]) == 0){
				if(nota >=5.0 && falta <25.0){
					conclusao = 1;
				}else{
					conclusao = 0;
				}return conclusao;	
			}				
		}		
	}
	if(strcmp(prerequisitos[0],"Nãohá") == 0){
		conclusao = 1;
	}

	fclose(fp);

	return conclusao;
}
void Matricula(int RA){


	int returne = 0;
    int semestre,sem_atual, c = 0;
    char disciplina[128][8];
	int requisito;// verifica se o aluno passou nos prerequisitos
	int creditos = 0; 
	float nota = 0, falta = 0;
	int ra = RA;

	do{
	    printf("\n####Menu de Realizar Matrícula#####\n\n");
		printf("Para sair digite XX000\n\n");
    	printf("Digite o semestre: \n");
		scanf("%d",&semestre);
		limparbuffer();
		sem_atual = Semestre(ra);
		creditos = Creditos(ra,semestre);// contagem creditos no semestre atual
		//printf("QNT CREDITOS: ###%d###\n",creditos);

		if(sem_atual > semestre || semestre == 0){
			printf("Semestre requerido inferior ao semestre mais atual\n");
			printf("Tente novamente:\n");
		}

	}while(sem_atual > semestre || semestre == 0);

	//printf("####Depoois do SEMESTRE do erro###\n\n");
	while(strcmp(disciplina[c],"XX000") != 0 && c <128){
		if (returne == 1){
			break;
		}

		printf("Digite a disciplina:\n");
		scanf("%s",disciplina[c]);
		limparbuffer();
		if(strcmp(disciplina[c],"XX000") == 0)
			break;
		requisito = Requisitos(ra,disciplina[c],semestre);
		switch(requisito){
			case 0:
				printf("Voce não possui os requisitos necessarios para cursar a disciplina %s\n",disciplina[c]);
				strcpy(disciplina[c],"SKIP");
				break;

			case 1:
				creditos += credito_disciplina(disciplina[c]);// credito por disciplina
				if(creditos > 32){
					printf("Limite de creditos excedido");
					returne = 1;
					break;
				}
				break;
			case 2:
				printf("Voce já esta matriculado ou já foi aprovado na disciplina %s\n",disciplina[c]);
				strcpy(disciplina[c],"SKIP");
				break;

			case 3:
				printf("Disciplina %s nao encontrada ou inexistente\n",disciplina[c]);
				strcpy(disciplina[c],"SKIP");
				break;
			default:
				break;		
		}				
		c++;
	}
	
	FILE  *fp = fopen("AlunosDisciplinas.txt","a+");
	for(int i = 0;i< c;i++){
		if(returne == 1)
			break;

			//printf("Disciplina: %d : ## %s ##\n",i,disciplina[i]);
			if(strcmp(disciplina[i],"SKIP") != 0){
				//printf("%d;%s;%d;%.1f;%.1f\n",ra,disciplina[i],semestre,nota,falta);
				fprintf(fp,"%d;%s;%d;%.1f;%.1f\n",ra,disciplina[i],semestre,nota,falta);
			}
	}
	fclose(fp);

	if (returne != 1)
	    printf("Transação efetuada com sucesso\n");
}

void Cadastro(){
    
    int ra;
    char nome[256];
    char login[32];
    char senha[16];

    FILE* fp = fopen("Alunos.txt","a");
    Aluno test;
    printf("Menu Cadastro de Alunos:\n\n");
    printf("Digite o RA do Aluno:\n");
    scanf("%d",&test.ra);
    printf("Digite o nome do Aluno:\n");
    limparbuffer();
    fgets(test.nome,256,stdin);
    printf("Digite o login do aluno:\n");
    fgets(test.login,32,stdin);
    printf("Digite a senha:\n");
    fgets(test.senha,16,stdin);
    
    test.nome[strlen(test.nome)-1]= '\0';
    test.login[strlen(test.login)-1]= '\0';
    test.senha[strlen(test.senha)-1]= '\0';

    fprintf(fp,"%d,",test.ra);
    fprintf(fp,"%s,",test.nome);
    fprintf(fp,"%s,",test.login);
    fprintf(fp,"%s\n",test.senha);
    fclose(fp);
}


int menu(int ra){
    int a;
	

	do{
	    printf("\nMenu de Opcoes\n\n");
	    printf("1- Cadastrar de Alunos\n");
	    printf("2- Consultar Disciplinas\n");
	    printf("3- Realizar Matrícula\n");
	    printf("4- Atualizar Nota e Falta\n");
		printf("5- Consultar Histórico do Aluno\n");
	    printf("0- Sair\n");
	
	    scanf("%d",&a);
	    limparbuffer();
	
	    switch(a){
    	    case 1:
    	        Cadastro();
    	        break;
        
    	    case 2:
    	        ConsultaD();        
    	        break;
    	    case 3:
    	        Matricula(ra);
    	        break;
    	    case 4:
    	        Notas(ra);
    	        break;
			case 5:
				ConsultaHistorico(ra);
				break;
    	    case 0:
    	        return 0;
	
    	    default:
    	        printf("Opcao inválida\n");
    	        break;
    	}
	}while(a != 0);
	return 0;

}

void Login(){

    char login[32], senha[16], nome[256], nomefake[256];
    int ra;
    // login == entrada o usuario senha == entrada do usuario
    printf("\nLogin:\n");
    fgets(login,32,stdin);
    login[strlen(login)-1] = '\0';

    printf("Senha:\n");
    fgets(senha,16,stdin);

    char log_correto[32];//login Coletados do ARquivo
    char senha_correta[16];/// senha arquivo
    char *sub;

    FILE *fp = fopen("Alunos.txt","r");
    while(fgets(nome,256,fp)){
        if(strcmp(nomefake,nome) == 0){// EOF nao funciona , se o nome se repetir ele quebra o laço
            break;
        }
        strcpy(nomefake,nome);
        sub = strtok(nome,",");// strtok verifica a aparicao de um delimitador no caso ','
        ra = atoi(sub);
        sub = strtok(NULL,",");
        strcpy(nome,sub);
        sub = strtok(NULL,",");
        strcpy(log_correto,sub);
        sub = strtok(NULL,",");
        strcpy(senha_correta,sub);


        if(strcmp(login,log_correto) == 0 && strcmp(senha,senha_correta) == 0){// compara login e senha de uma mesma linha dentro do arquivo com a entrada do usuario
            printf("\nBem vindo %s\n\n", nome);
            menu(ra);
            break;
        }
    }

    fclose(fp);
    if(strcmp(login,log_correto) != 0 && strcmp(senha,senha_correta) != 0){
        printf("Usuario ou senha inválidas\n");
    }
}

int ConsultaD(){
    char linha[256];
    char linha_fake[256];// linha fake mesma logica nome fake
    char codigo[30];
    char nome[50],nomeP[50];// nomep == nome prerequisito
    int creditos,codS = 0,creditoP;// cods == verificacao de algum erro ou invalides creditoP == credito do prerequisito
    char Prerequisito[2][60];
    char *sub;
    char cod_correto[8];

    printf("\nDigite o Codigo da disciplina: ");
    fgets(codigo, 8, stdin);
    codigo[strlen(codigo)-1]= '\0';

    FILE* fp= fopen("Disciplinas.txt","r");
    int c = 0;

    while(fgets(linha, 256, fp)){
        if(strcmp(linha,linha_fake) == 0){
            break;
        }
        strcpy(linha_fake,linha);
        sub = strtok(linha,";");
        strcpy(cod_correto,sub);
        sub = strtok(NULL,";");
        strcpy(nome,sub);
        sub = strtok(NULL,";");
        creditos = atoi(sub);
        fscanf(fp,"\n");

        if(strcmp(codigo,cod_correto) == 0 ){
            codS = 1;// inutil
            break;
        }
    
    }if(codS != 1){
        printf("Codigo invalido\n");
        ConsultaD();
    }
        

    codS = 0;
    fclose(fp);// fecha disciplinas
    fp = fopen("Prerequisitos.txt","r");
    int b = 1;
    while(fgets(linha, 256, fp)){//EOF
        if(strcmp(linha_fake, linha) == 0)// ler duas linhas iguais quebra o laço
            break;
        strcpy(linha_fake, linha);
    //real>>
        //printf("%s\n", linha);
        sub = strtok(linha,";");
        strcpy(cod_correto,sub);
        sub = strtok(NULL,";");

        //printf("%s\n", sub);
        //strcpy(Prerequisito,sub);
        
        if(strcmp(cod_correto,codigo)== 0){//teste de codigo de entrada e codigo
                                                            //lido do arquivo se equivalente quebra o laco e permaneçe no codigo atual lido
        int j = 0;
        int i = 0;
        
            if(sub[5] == ' '){
                sscanf(sub,"%s %s",Prerequisito[0],Prerequisito[1]);
                //printf("teste: %s e %s\n", Prerequisito[0],Prerequisito[1]);
                b = 2;
            }else{
                strcpy(Prerequisito[0],sub);
                //printf("teste: %s\n", Prerequisito[b]);
            }

            codS = 1;
            break;
        }
        Prerequisito[b][strlen(Prerequisito[b])-1] = '\0';
        // printf("valor de b:%d\n", b);

    }if(codS != 1){
        printf("Codigo de disciplina invalido\n");
        ConsultaD();
    }

    fclose(fp);
    int i = 0;
    char *codis[2];
    for(int i = 0; i < 2; i++){
        codis[i] = (char*)malloc(sizeof(char)*5);
    }
    char nomes[2][100];

    //if(strcmp(Prerequisito[b],"Nãohá") == 0){//pre requisito
        // strcpy(nomeP,"Não há");
        // strcpy(nomes[0],"Não há");
    // }else{

    for(i = 0; i < b; i++){
        fp = fopen("Disciplinas.txt","r");
        strcpy(codigo,Prerequisito[i]);
        // printf("%s!\n", codigo);
        
        if (codigo[strlen(codigo) - 1] == '\n')
            codigo[strlen(codigo) - 1] = '\0';

        while(fgets(linha,256,fp)){
            if(strcmp(linha_fake,linha) == 0)
                break;

            strcpy(linha_fake,linha);
            sub = strtok(linha,";");
            strcpy(cod_correto,sub);
            sub = strtok(NULL,";");
            strcpy(nomeP,sub);
            sub = strtok(NULL,";");
            // printf("Pre:%s", nomeP);
            creditoP = atoi(sub);
            fscanf(fp,"\n");
            
            // printf(" \t%s -  %s\t ", cod_correto, codigo);
            if(strcmp(cod_correto,codigo)== 0 ){
                codS = 1;
                break;
            }
        }

        if(codS != 1){
            printf("Codigo de Prerequisito invalido\n");
            ConsultaD();
            fclose(fp);
        }else{
            // printf("teste do codigo %s: \n",codigo);
            strcpy(codis[i],codigo);
            // printf("teste do codigo 2: %s",codis[0]);
            strcpy(nomes[i],nomeP);            
        }
    }
    
    printf("\nNome: %s\n",nome);
    printf("Quantidade de Créditos: %d\n",creditos);
    // printf("Pré-requisitos: %s - %s\n",codigo,nomeP);
    printf("Pré-requisitos: ");
    if(strcmp(codis[0],"Nãohá") == 0){
        printf("Não há requisitos.\n");
    }else{    
        for(int i = 0; i < b; i++){
            printf("%s - %s \n",codis[i],nomes[i]);
        }
    }
    fclose(fp);
    codS = 0;
    
    menu(codS);
}

int main(){

    FILE * fp = fopen("Alunos.txt","r");
    if(fp == NULL){
        fp = fopen("Alunos.txt","w");
        fprintf(fp,"478596,Marcelo Stonks,1,1\n");
        fclose(fp);
    }

	do{
    	printf("\nSistema de Gerência de Disciplinas Semestrais\n\n");
    	printf("1- Cadastro de Aluno\n");
    	printf("2- Login\n");
    	printf("0- Sair do menu\n");
    	printf("Digite a opção:\n");
    	scanf("%d",&opcao);
    	limparbuffer();

    	switch(opcao){
        	case 0:
        	    return 0;

        	case 1:
        	    Cadastro();        
        	    break;
        
        	case 2:
        	    Login();
        	    break;

        	default:
        	    printf("Opcao inválida");
        	    break;
    	}
	}while(opcao != 0);

    return 0;
}
