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
  %i2 = load i32, i32* %i, align 4
  %0 = icmp slt i32 %i2, 0
  %1 = zext i1 %0 to i32
  %2 = icmp eq i32 %1, 1
  br i1 %2, label %s_true, label %select_

exit_select:                                      ; preds = %select_3, %s_true4, %s_true
  %i6 = load i32, i32* %i, align 4
  ret i32 %i6

select_:                                          ; preds = %enter
  %i5 = load i32, i32* %i, align 4
  %3 = icmp eq i32 %i5, 0
  %4 = zext i1 %3 to i32
  %5 = icmp eq i32 %4, 1
  br i1 %5, label %s_true4, label %select_3

s_true:                                           ; preds = %enter
  %6 = sub nsw i32 0, 1
  ret i32 %6
  br label %exit_select

select_3:                                         ; preds = %select_
  br label %exit_select

s_true4:                                          ; preds = %select_
  ret i32 99
  br label %exit_select
}
