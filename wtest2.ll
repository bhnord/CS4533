; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

@s = dso_local global i8* getelementptr inbounds ([6 x i8], [6 x i8]* @s.1, i32 0, i32 0), align 8
@s.1 = private unnamed_addr constant [6 x i8] c"hello\00", align 1

define dso_local i8* @c() {
chead:
  br label %enter

enter:                                            ; preds = %chead
  %s = load i8*, i8** @s, align 8
  ret i8* %s
}
