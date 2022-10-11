; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

define i32 @c(i32 %k) {
chead:
  %k1 = alloca i32, align 4
  store i32 %k, i32* %k1, align 4
  br label %block

block:                                            ; preds = %chead
  %0 = icmp slt i32 2, 5
  %1 = zext i1 %0 to i32
  %2 = icmp eq i32 %1, 1
  br i1 %2, label %then, label %else

then:                                             ; preds = %block
  br label %block3

cont:                                             ; preds = %block3, %block2
  %k4 = load i32, i32* %k1, align 4
  ret i32 %k4

else:                                             ; preds = %block
  br label %block2

block2:                                           ; preds = %else
  %3 = sub nsw i32 0, 1
  ret i32 %3
  br label %cont

block3:                                           ; preds = %then
  ret i32 100
  br label %cont
}
