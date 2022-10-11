; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

@tt = common global i8* null, align 8
@ss = common global i32 999, align 4

define i8* @c(i8* %n) {
chead:
  %n1 = alloca i8*, align 8
  store i8* %n, i8** %n1, align 8
  br label %enter

enter:                                            ; preds = %chead
  %n2 = load i8*, i8** %n1, align 8
  ret i8* %n2
}
