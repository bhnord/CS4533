; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

declare i32 @printf(i8*, ...)

define dso_local i32 @program() {
programhead:
  br label %enter

enter:                                            ; preds = %programhead
  %arr = alloca [5 x i32], align 16
  %k = alloca i32, align 4
  store i32 3, i32* %k, align 4
  %0 = getelementptr inbounds [5 x i32], [5 x i32]* %arr, i32 0, i32 3
  %1 = load i32, i32* %0, align 4
  store i32 %1, i32* %k, align 4
  ret i32 5
}
