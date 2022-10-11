; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

@k = common global i32 0, align 4

define i32 @c(i32 %f) {
chead:
  %f1 = alloca i32, align 4
  store i32 %f, i32* %f1, align 4
  br label %enter

enter:                                            ; preds = %chead
  %r = alloca i32, align 4
  store i32 0, i32* %r, align 4
  %0 = add nsw i32 3, 5
  store i32 %0, i32* %r, align 4
  %r2 = load i32, i32* %r, align 4
  ret i32 %r2
}
