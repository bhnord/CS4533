; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

declare i8* @printf(...)

define i32 @main(i32 %0, i8** %1) {
entry:
  %k = alloca i32, align 4
  store i32 4, i32* %k, align 4
  ret i32 0
}

declare i32 @t(i32, i32)

define i32 @t.1(i32 %k, i32 %s) {
thead:
  br label %entry

entry:                                            ; preds = %thead
  %s1 = alloca i32, align 4
  store i32 0, i32* %s1, align 4
  ret i32 4
}
