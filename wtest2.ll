; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

@k = common global i32 0, align 4

define i32 @c(i32 %f) {
chead:
  %f1 = alloca i32, align 4
  store i32 %f, i32* %f1, align 4
  br label %block

block:                                            ; preds = %chead
  %i = alloca i32, align 4
  store i32 0, i32* %i, align 4
  br label %loop

exit_loop:                                        ; preds = %loop
  %i5 = load i32, i32* %i, align 4
  ret i32 %i5

loop:                                             ; preds = %block2, %block
  %i4 = load i32, i32* %i, align 4
  %0 = icmp eq i32 %i4, 0
  %1 = zext i1 %0 to i32
  %2 = icmp eq i32 %1, 1
  br i1 %2, label %block2, label %exit_loop

block2:                                           ; preds = %loop
  %i3 = load i32, i32* %i, align 4
  %3 = add nsw i32 %i3, 1
  store i32 %3, i32* %i, align 4
  br label %loop
}
