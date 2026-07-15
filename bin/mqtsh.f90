program mqtsh
    implicit none

    integer, parameter :: MAX_CMD_LEN = 4096
    integer, parameter :: MAX_VARS = 1024
    character(len=*), parameter :: VERSION = '0.2'
    character(len=256), dimension(MAX_VARS) :: var_names
    character(len=MAX_CMD_LEN), dimension(MAX_VARS) :: var_values
    integer :: var_count = 0
    character(len=MAX_CMD_LEN) :: line
    logical :: running = .true.
    character(len=256) :: arg
    integer :: i, stat, eq_pos
    logical :: show_version = .false.
    logical :: show_help = .false.
    character(len=1024) :: config_file
    logical :: has_config = .false.
    character(len=256) :: var_name, var_value

    do i = 1, command_argument_count()
        call get_command_argument(i, arg)
        select case (trim(arg))
        case ('-h', '--help')
            show_help = .true.
        case ('-v', '--version')
            show_version = .true.
        case ('-s', '--source')
            call get_command_argument(i+1, config_file)
            has_config = .true.
        end select
    end do

    if (show_help) then
        write(*,*) 'mqtsh - The Mesquite Shell'
        write(*,*) 'Usage: mqtsh [options]'
        write(*,*) ''
        write(*,*) 'Options:'
        write(*,*) '  -h, --help     Display this help message'
        write(*,*) '  -v, --version  Display version information'
        write(*,*) '  -s, --source   Specify config file to source'
        write(*,*) ''
        write(*,*) 'Objective-C Style Scripting:'
        write(*,*) '  [@"command"]        - Execute command'
        write(*,*) '  [<var> = @"value"]  - Variable assignment'
        write(*,*) '  @import "file.sh"   - Import script'
        write(*,*) '  @function name { } - Define function'
        stop
    end if

    if (show_version) then
        write(*,*) 'mqtsh '//VERSION//' (POSIX-compliant shell in GNU Fortran)'
        stop
    end if

    if (has_config) then
        call run_script(trim(config_file))
    end if

    do while (running)
        write(*, '(a)', advance='no') 'mqtsh-'//VERSION//'# '
        read(*, '(a)', iostat=stat) line
        if (stat /= 0) exit

        call process_input(trim(line))
    end do

contains

    function get_var(name) result(value)
        character(len=*), intent(in) :: name
        character(len=MAX_CMD_LEN) :: value
        integer :: j

        value = ''
        do j = 1, var_count
            if (trim(var_names(j)) == trim(name)) then
                value = trim(var_values(j))
                return
            end if
        end do
    end function get_var

    subroutine set_var(name, value)
        character(len=*), intent(in) :: name, value
        integer :: j

        do j = 1, var_count
            if (trim(var_names(j)) == trim(name)) then
                var_values(j) = trim(value)
                return
            end if
        end do

        var_count = var_count + 1
        if (var_count <= MAX_VARS) then
            var_names(var_count) = trim(name)
            var_values(var_count) = trim(value)
        end if
    end subroutine set_var

    subroutine run_script(filename)
        character(len=*), intent(in) :: filename
        character(len=MAX_CMD_LEN) :: script_line
        integer :: unit, ios

        open(newunit=unit, file=trim(filename), status='old', action='read', iostat=ios)
        if (ios /= 0) then
            write(*,*) 'mqtsh: cannot open '//trim(filename)
            return
        end if

        do
            read(unit, '(a)', iostat=ios) script_line
            if (ios /= 0) exit
            call process_input(trim(script_line))
        end do

        close(unit)
    end subroutine run_script

    subroutine handle_import(line)
        character(len=*), intent(in) :: line
        character(len=1024) :: filename
        integer :: idx, j

        idx = index(line, '"')
        if (idx == 0) return
        j = idx + 1
        idx = 1
        do while (j <= len(trim(line)) .and. line(j:j) /= '"')
            filename(idx:idx) = line(j:j)
            idx = idx + 1
            j = j + 1
        end do

        call run_script(trim(filename))
    end subroutine handle_import

    subroutine process_input(line)
        character(len=*), intent(in) :: line

        if (len_trim(line) == 0) return
        if (line(1:1) == '!') return

        if (line(1:7) == '@import') then
            call handle_import(line)
        else if (line(1:1) == '[') then
            call parse_objc_bracket(line)
        else if (trim(line) == 'exit') then
            running = .false.
        else if (line(1:3) == 'cd ') then
            call system('cd "'//trim(line(4:))//'"')
        else if (line(1:5) == 'echo ') then
            write(*,*) trim(line(6:))
        else
            eq_pos = index(line, '=')
            if (eq_pos > 0) then
                call set_var(adjustl(line(1:eq_pos-1)), adjustl(line(eq_pos+1:)))
            else
                call system(trim(line))
            end if
        end if
    end subroutine process_input

    subroutine parse_objc_bracket(line)
        character(len=*), intent(in) :: line
        character(len=MAX_CMD_LEN) :: cmd
        integer :: pos1, pos2

        if (index(line, '@"') > 0) then
            cmd = ''
            pos1 = index(line, '@"')
            if (pos1 > 0) then
                pos2 = pos1 + 2
                do while (pos2 <= len_trim(line) .and. line(pos2:pos2) /= '"')
                    cmd = trim(cmd) // line(pos2:pos2)
                    pos2 = pos2 + 1
                end do
                call system(trim(cmd))
            end if
        else if (index(line, '= @"') > 0) then
            call parse_objc_assignment(line)
        end if
    end subroutine parse_objc_bracket

    subroutine parse_objc_assignment(line)
        character(len=*), intent(in) :: line
        character(len=256) :: vname, vvalue
        integer :: eq_pos, pos1, pos2

        eq_pos = index(line, '=')
        if (eq_pos == 0) return

        vname = trim(adjustl(line(2:eq_pos-1)))

        pos1 = eq_pos + 3
        pos2 = 1
        do while (pos1 <= len_trim(line) .and. pos2 <= MAX_CMD_LEN)
            if (line(pos1:pos1) == '"') exit
            vvalue(pos2:pos2) = line(pos1:pos1)
            pos2 = pos2 + 1
            pos1 = pos1 + 1
        end do
        vvalue(pos2:pos2) = ''
        vvalue = trim(vvalue)
        call set_var(adjustl(vname), trim(vvalue))
    end subroutine parse_objc_assignment

end program mqtsh
