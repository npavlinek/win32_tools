require 'erb'

signed_types = ['signed char', 'signed short', 'signed int', 'signed long', 'signed long long']
unsigned_types = ['unsigned char', 'unsigned short', 'unsigned int', 'unsigned long', 'unsigned long long']

program_template = <<~EOF
  #include <stddef.h>
  #include <stdio.h>

  int main(void)
  {
      int found_int8 = 0;
      int found_int16 = 0;
      int found_int32 = 0;
      int found_int64 = 0;
      int found_uint8 = 0;
      int found_uint16 = 0;
      int found_uint32 = 0;
      int found_uint64 = 0;

      puts("#ifndef FIXED_WIDTH_INTEGERS_H");
      puts("#define FIXED_WIDTH_INTEGERS_H\\n");

  <% signed_types.each do |type| -%>
      switch (sizeof(<%= type %>))
      {
      case 1:
          if (!found_int8)
          {
              puts("typedef <%= type %> int8;");
              found_int8 = 1;
          }
          break;
      case 2:
          if (!found_int16)
          {
              puts("typedef <%= type %> int16;");
              found_int16 = 1;
          }
          break;
      case 4:
          if (!found_int32)
          {
              puts("typedef <%= type %> int32;");
              found_int32 = 1;
          }
          break;
      case 8:
          if (!found_int64)
          {
              puts("typedef <%= type %> int64;");
              found_int64 = 1;
          }
          break;
      }

  <% end -%>

      puts("");

  <% unsigned_types.each do |type| -%>
      switch (sizeof(<%= type %>))
      {
      case 1:
          if (!found_uint8)
          {
              puts("typedef <%= type %> uint8;");
              found_uint8 = 1;
          }
          break;
      case 2:
          if (!found_uint16)
          {
              puts("typedef <%= type %> uint16;");
              found_uint16 = 1;
          }
          break;
      case 4:
          if (!found_uint32)
          {
              puts("typedef <%= type %> uint32;");
              found_uint32 = 1;
          }
          break;
      case 8:
          if (!found_uint64)
          {
              puts("typedef <%= type %> uint64;");
              found_uint64 = 1;
          }
          break;
      }

  <% end -%>

      puts("");

  <% signed_types.each do |type| -%>
      switch (sizeof(<%= type %>))
      {
      case 8:
          puts("typedef <%= type %> intptr;");
          break;
      default:
          break;
      }

  <% end -%>

  <% unsigned_types.each do |type| -%>
      switch (sizeof(<%= type %>))
      {
      case 8:
          puts("typedef <%= type %> uintptr;");
          break;
      default:
          break;
      }

  <% end -%>

      puts("\\n#endif /* FIXED_WIDTH_INTEGERS_H */");
      return 0;
  }
EOF

ERB.new(program_template, trim_mode: '-').run
