/*レジスタがマッピングされているアドレス*/
#define REG_ADDRESS 0170000

/*pre K&R Cでは無名構造体が許されている。その要素はどの変数に対しても使用出来る。*/
struct {
    int integ;;
};

main() {
    int a;
    a = REG_ADDRESS->integ; /*レジスタの値読み出し*/
    REG_ADDRESS->integ =0; /*レジスタへ値書き込み（無名構造体の要素へアクセス）*/
}