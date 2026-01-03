#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define SIZE 8

// ------------- utils -----------------
int isWhite(char p){ return p>='A' && p<='Z'; }
int isBlack(char p){ return p>='a' && p<='z'; }
int isValid(int r,int c){ return r>=0 && r<SIZE && c>=0 && c<SIZE; }
int rowToIndex(char c){ return c-'1'; }
int colToIndex(char c){ return c-'a'; }

// ------------- board -----------------
void printBoard(char b[SIZE][SIZE]){
    printf("\n");
    for(int r=SIZE-1;r>=0;r--){
        printf("%d | ", r+1);
        for(int c=0;c<SIZE;c++) printf("%c ", b[r][c]);
        printf("\n");
    }
    printf("  _ ");
    for(int c=0;c<SIZE*2-1;c++) printf("_");
    printf("\n    ");
    for(int c=0;c<SIZE;c++) printf("%c ", 'a'+c);
    printf("\n");
}

int clearPath(char b[SIZE][SIZE],int fr,int fc,int tr,int tc){
    int dr=(tr>fr)-(tr<fr), dc=(tc>fc)-(tc<fc);
    fr+=dr; fc+=dc;
    while(fr!=tr || fc!=tc){
        if(b[fr][fc]!='.') return 0;
        fr+=dr; fc+=dc;
    }
    return 1;
}

int countDiagonalPieces(char b[SIZE][SIZE], int fr,int fc,int tr,int tc){
    int dr=(tr>fr)-(tr<fr), dc=(tc>fc)-(tc<fc);
    fr+=dr; fc+=dc;
    int cnt=0;
    while(fr!=tr || fc!=tc){
        if(b[fr][fc]!='.') cnt++;
        fr+=dr; fc+=dc;
    }
    return cnt;
}

// ------------- rules / pieces -----------------
int isCheck(char b[SIZE][SIZE], int whiteTurn){
    // بررسی کیش: شاه مقابل می‌تواند گرفته شود
    int kr=-1,kc=-1;
    char king = whiteTurn?'K':'k';
    for(int r=0;r<SIZE;r++)
        for(int c=0;c<SIZE;c++)
            if(b[r][c]==king){ kr=r; kc=c; break; }
    if(kr==-1) return 0; // شاه پیدا نشد
    // بررسی تهدید توسط همه مهره‌های حریف
    for(int r=0;r<SIZE;r++)
        for(int c=0;c<SIZE;c++){
            char p=b[r][c];
            if(p=='.') continue;
            if(whiteTurn && isWhite(p)) continue;
            if(!whiteTurn && isBlack(p)) continue;
            int dr=kr-r, dc=kc-c;
            switch(toupper(p)){
                case 'G': if(abs(dr)==abs(dc) && clearPath(b,r,c,kr,kc)) return 1; break;
                case 'D': if((abs(dr)==2 && abs(dc)==1)||(abs(dr)==1 && abs(dc)==2)||(abs(dr)<=1 && abs(dc)<=1)) return 1; break;
                case 'T': if(abs(dr)==abs(dc) && countDiagonalPieces(b,r,c,kr,kc)==1) return 1; break;
                case 'Q': if(((dr==0||dc==0||abs(dr)==abs(dc)) && clearPath(b,r,c,kr,kc))||((abs(dr)==2 && abs(dc)==1)||(abs(dr)==1 && abs(dc)==2))) return 1; break;
                case 'K': if(abs(dr)<=2 && abs(dc)<=2) return 1; break;
                case 'P':
                    if(isWhite(p) && dr==1 && abs(dc)==1 && b[kr][kc]!='.') return 1;
                    if(isBlack(p) && dr==-1 && abs(dc)==1 && b[kr][kc]!='.') return 1;
                    break;
            }
        }
    return 0;
}

int hasAnyLegalMoves(char b[SIZE][SIZE], int whiteTurn);

int isCheckmate(char b[SIZE][SIZE], int whiteTurn){
    return isCheck(b,whiteTurn) && !hasAnyLegalMoves(b,whiteTurn);
}

int isStalemate(char b[SIZE][SIZE], int whiteTurn){
    return !isCheck(b,whiteTurn) && !hasAnyLegalMoves(b,whiteTurn);
}

int insufficientMaterial(char b[SIZE][SIZE]){
    // ساده‌سازی: اگر فقط شاهها باقی مانده باشند، مساوی
    int pieces=0;
    for(int r=0;r<SIZE;r++)
        for(int c=0;c<SIZE;c++)
            if(b[r][c]!='.' && toupper(b[r][c])!='K') pieces++;
    return pieces==0;
}

// بررسی اینکه حداقل یک حرکت قانونی وجود دارد
int isLegalMove(char b[SIZE][SIZE], int fR,int fC,int tR,int tC,int whiteTurn){
    if(!isValid(fR,fC)||!isValid(tR,tC)) return 0;
    char p=b[fR][fC], d=b[tR][tC];
    if(p=='.') return 0;
    if(whiteTurn && !isWhite(p)) return 0;
    if(!whiteTurn && !isBlack(p)) return 0;
    if(d!='.' && ((isWhite(p)&&isWhite(d))||(isBlack(p)&&isBlack(d)))) return 0;

    int dr=tR-fR, dc=tC-fC, ok=0;
    switch(toupper(p)){
        case 'G': {
            int mr=fR+(dr>0?1:-1), mc=fC+(dc>0?1:-1);
            if(abs(dr)>=1 && abs(dc)>=1 && (mr==tR||mc==tC) && b[mr][mc]=='.' && clearPath(b,mr,mc,tR,tC)) ok=1;
            break;
        }
        case 'D':
            ok=(abs(dr)==2&&abs(dc)==1)||(abs(dr)==1&&abs(dc)==2)||(abs(dr)<=1&&abs(dc)<=1);
            break;
        case 'T':
            if(abs(dr)==abs(dc) && countDiagonalPieces(b,fR,fC,tR,tC)==1) ok=1;
            break;
        case 'Q':
            ok=((dr==0||dc==0||abs(dr)==abs(dc)) && clearPath(b,fR,fC,tR,tC)) ||
               ((abs(dr)==2&&abs(dc)==1)||(abs(dr)==1&&abs(dc)==2));
            break;
        case 'K':
            ok=(abs(dr)<=2 && abs(dc)<=2 && !(abs(dr)==2 && abs(dc)==2) && clearPath(b,fR,fC,tR,tC));
            break;
        case 'P':
            if(isWhite(p)){
                if(dc==0 && dr==1 && d=='.') ok=1;
                else if(dc==0 && dr==2 && fR==1 && b[fR+1][fC]=='.' && d=='.') ok=1;
                else if(abs(dc)==1 && dr==1 && isBlack(d)) ok=1;
            } else {
                if(dc==0 && dr==-1 && d=='.') ok=1;
                else if(dc==0 && dr==-2 && fR==6 && b[fR-1][fC]=='.' && d=='.') ok=1;
                else if(abs(dc)==1 && dr==-1 && isWhite(d)) ok=1;
            }
            break;
    }

    if(!ok) return 0;

    char temp=b[tR][tC]; b[fR][fC]='.'; b[tR][tC]=p;
    int check=isCheck(b,whiteTurn);
    b[fR][fC]=p; b[tR][tC]=temp;

    return !check;
}

int hasAnyLegalMoves(char b[SIZE][SIZE], int whiteTurn){
    for(int r=0;r<SIZE;r++)
        for(int c=0;c<SIZE;c++)
            if((whiteTurn && isWhite(b[r][c])) || (!whiteTurn && isBlack(b[r][c])))
                for(int tr=0;tr<SIZE;tr++)
                    for(int tc=0;tc<SIZE;tc++)
                        if(isLegalMove(b,r,c,tr,tc,whiteTurn))
                            return 1;
    return 0;
}

// ------------- main -----------------
int main(){
    char b[SIZE][SIZE];
    int whiteTurn=1;

    for(int r=0;r<SIZE;r++)
        for(int c=0;c<SIZE;c++) b[r][c]='.';

    char w[]="GDTQKTDG"; char bl[]="gdtqktdg";
    for(int c=0;c<SIZE;c++){
        b[0][c]=w[c]; b[1][c]='P';
        b[6][c]='p'; b[7][c]=bl[c];
    }

    printBoard(b);

    while(1){
        char from[3], to[3];
        if(whiteTurn) printf("White to move: "); else printf("Black to move: ");
        scanf("%s %s", from, to);

        if(from[0]=='a' && from[1]=='9' && to[0]=='a' && to[1]=='9') break;

        int fC=colToIndex(from[0]), fR=rowToIndex(from[1]);
        int tC=colToIndex(to[0]), tR=rowToIndex(to[1]);

        if(!isLegalMove(b,fR,fC,tR,tC,whiteTurn)){
            printf("Invalid move\n");
            continue;
        }

        char p=b[fR][fC]; b[fR][fC]='.'; b[tR][tC]=p;

        // ارتقای پیاده با انتخاب کاربر
        if((p=='P' && tR==7) || (p=='p' && tR==0)){
            char choice;
            do{
                printf("Promote pawn to (Q,R,T,D): ");
                scanf(" %c",&choice);
                choice=toupper(choice);
            }while(strchr("QRTD",choice)==NULL);
            if(p=='P') b[tR][tC]=choice;
            else b[tR][tC]=tolower(choice);
        }

        printBoard(b);

        int opponent=!whiteTurn;
        if(isCheckmate(b,opponent)){ printf("Checkmate! %s wins.\n", whiteTurn?"White":"Black"); break; }
        if(isStalemate(b,opponent) || insufficientMaterial(b)){ printf("Draw.\n"); break; }
        if(isCheck(b,opponent)) printf("Check!\n");

        whiteTurn=opponent;
    }

    printf("Game ended.\n");
    return 0;
}
