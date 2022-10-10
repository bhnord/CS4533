; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

@k = external global i32

declare i8* @printf(...)

define i32 @c(i32 %s) {
chead:
  br label %block

block:                                            ; preds = %chead
  %s1 = load i32, i32 %s, align 4
  store i32 %s1, i32* @k, align 4
  ret i32 4
}
