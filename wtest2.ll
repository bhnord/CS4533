; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

@k = common global i32 0, align 4

define i32 @c(i32 %f) {
chead:
  %f1 = alloca i32, align 4
  store i32 %f, i32* %f1, align 4
  br label %enter

enter:                                            ; preds = %chead
  %i = alloca i32, align 4
  store i32 0, i32* %i, align 4
  br label %check_loop

exit_loop:                                        ; preds = %check_loop
  %i4 = load i32, i32* %i, align 4
  ret i32 %i4

check_loop:                                       ; preds = %loop, %enter
  %i2 = load i32, i32* %i, align 4
  %0 = icmp eq i32 %i2, 0
  %1 = zext i1 %0 to i32
  %2 = icmp eq i32 %1, 1
  br i1 %2, label %loop, label %exit_loop

loop:                                             ; preds = %check_loop
  %i3 = load i32, i32* %i, align 4
  %3 = add nsw i32 %i3, 1
  store i32 %3, i32* %i, align 4
  br label %check_loop
}
