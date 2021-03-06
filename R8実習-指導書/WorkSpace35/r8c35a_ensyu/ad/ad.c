/****************************************************************************/
/* 対象マイコン R8C/35A                                                     */
/* ﾌｧｲﾙ内容     A/D変換(単発モード)                                         */
/* バージョン   Ver.1.20                                                    */
/* Date         2010.04.19                                                  */
/* Copyright    ルネサスマイコンカーラリー事務局                            */
/*              日立インターメディックス株式会社                            */
/****************************************************************************/
/*
入力：AN7(P0_0)端子 0〜5V(ミニマイコンカーの赤外線フォトインタラプタU8)
出力：P1_3-P1_0(マイコンボードのLED)

AN7(P0_0)端子から入力した電圧をA/D変換して、デジタル値をマイコンボードの
LEDへ出力します。
*/

/*======================================*/
/* インクルード                         */
/*======================================*/
#include "sfr_r835a.h"                  /* R8C/35A SFRの定義ファイル    */

/*======================================*/
/* シンボル定義                         */
/*======================================*/

/*======================================*/
/* プロトタイプ宣言                     */
/*======================================*/
void init( void );
int get_ad7( void );
void led_out( unsigned char led );

/************************************************************************/
/* メインプログラム                                                     */
/************************************************************************/
void main( void )
{
    int ad;

    init();                             /* 初期化                       */

    while( 1 ) {
        ad = get_ad7();
        ad = ad >> 6;
        led_out( ad );
    }
}

/************************************************************************/
/* R8C/35A スペシャルファンクションレジスタ(SFR)の初期化                */
/************************************************************************/
void init( void )
{
    int i;

    /* クロックをXINクロック(20MHz)に変更 */
    prc0  = 1;                          /* プロテクト解除               */
    cm13  = 1;                          /* P4_6,P4_7をXIN-XOUT端子にする*/
    cm05  = 0;                          /* XINクロック発振              */
    for(i=0; i<50; i++ );               /* 安定するまで少し待つ(約10ms) */
    ocd2  = 0;                          /* システムクロックをXINにする  */
    prc0  = 0;                          /* プロテクトON                 */

    /* ポートの入出力設定 */
    prc2 = 1;                           /* PD0のプロテクト解除          */
    pd0 = 0xe0;                         /* 7-5:LED 4:SW 3-0:アナログ電圧*/
    p1  = 0x0f;                         /* 3-0:LEDは消灯                */
    pd1 = 0xdf;                         /* 5:RXD0 4:TXD0 3-0:LED        */
    pd2 = 0xfe;                         /* 0:PushSW                     */
    pd3 = 0xfb;                         /* 4:Buzzer 2:IR                */
    pd4 = 0x83;                         /* 7:XOUT 6:XIN 5-3:DIP SW 2:VREF*/
    pd5 = 0x40;                         /* 7:DIP SW                     */
    pd6 = 0xff;
}

/************************************************************************/
/* A/D値読み込み(AN7)                                                   */
/* 引数　 なし                                                          */
/* 戻り値 A/D値 0〜1023                                                 */
/************************************************************************/
int get_ad7( void )
{
    int i;

    /* A/Dコンバータの設定 */
    admod   = 0x03;                     /* 単発モードに設定             */
    adinsel = 0x07;                     /* 入力端子AN7(P0_0)を選択      */
    adcon1  = 0x30;                     /* A/D動作可能                  */
    asm(" nop ");                       /* φADの1サイクルウエイト入れる*/
    adcon0  = 0x01;                     /* A/D変換スタート              */

    while( adcon0 & 0x01 );             /* A/D変換終了待ち              */

    i = ad7;

    return i;
}

/************************************************************************/
/* マイコン部のLED出力                                                  */
/* 引数　 スイッチ値 0〜15                                              */
/************************************************************************/
void led_out( unsigned char led )
{
    unsigned char data;

    led = ~led;
    led &= 0x0f;
    data = p1 & 0xf0;
    p1 = data | led;
}

/************************************************************************/
/* end of file                                                          */
/************************************************************************/
