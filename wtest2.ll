; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

@const = private unnamed_addr constant [11 x i8] c"helloworld\00", align 1

declare i32 @printf(i8*, ...)

define dso_local i8* @program() {
programhead:
  br label %enter

enter:                                            ; preds = %programhead
  %arr = alloca [5 x i8*], align 16
  %k = alloca i32, align 4
  store i32 3, i32* %k, align 4
  %0 = getelementptr inbounds [5 x i8*], [5 x i8*]* %arr, i32 0, i32 3
  store i8* getelementptr inbounds ([11 x i8], [11 x i8]* @const, i32 0, i32 0), i8** %0, align 8
  %1 = getelementptr inbounds [5 x i8*], [5 x i8*]* %arr, i32 0, i32 3
  %2 = load i8*, i8** %1, align 8
  ret i8* %2
}
