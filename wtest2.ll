; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

@const = private unnamed_addr constant [6 x i8] c"Hello\00", align 1
@const.1 = private unnamed_addr constant [13 x i8] c"line1\\nline2\00", align 1

declare i32 @printf(i8*, ...)

define dso_local i8* @program() {
programhead:
  br label %enter

enter:                                            ; preds = %programhead
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @const, i32 0, i32 0))
  ret i8* getelementptr inbounds ([13 x i8], [13 x i8]* @const.1, i32 0, i32 0)
}
