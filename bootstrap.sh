#! /bin/sh
# July 13, 2019

platform() {
    case $OSTYPE in
    linux-gnu)
        if [ -f /etc/os-release ]
        then
            . /etc/os-release

            case $ID in
            debian|ubuntu|raspbian)
                echo "linux-deb"
                ;;
            fedora|centos)
                echo "linux-rpm"
                ;;
            *)
                echo "ERROR: Unsupported Linux OS: $ID" >&2
                return 1
            esac
        else
            echo "ERROR: Unsupported Linux OS: no such file '/etc/os-release'" >&2
            return 1
        fi
        ;;

    darwin*)
        echo "darwin"
        ;;

    FreeBSD)
        echo "freebsd"
        ;;

    "")
        echo "ERROR: Undefined variable OSTYPE" >&2

        if [ "$LOGNAME" = "root" ]
        then
            echo "If you are using sudo, preserve user environment: sudo -E $0" >&2
        fi

        return 1
        ;;

    *)
        echo "ERROR: Unsupported platform: $OSTYPE" >&2
        return 1
    esac

    return 0
}

PLATFORM=$(platform)

if [ $? != 0 ]
then
    exit 1
fi

install_cjson() {
    case $PLATFORM in
    linux-deb)
        apt-get install -y curl || exit $?
        ;;

    linux-rpm)
        yum install -y curl || exit $?
    esac

    TAG=$(curl -s https://api.github.com/repos/DaveGamble/cJSON/releases/latest | grep -oP '"tag_name": "\K(.*)(?=")') || exit $?
    VERSION=${TAG#v}

    curl -OL https://github.com/DaveGamble/cJSON/archive/$TAG.tar.gz || exit $?
    tar -zxf $TAG.tar.gz
    cd cJSON-$VERSION
    make && make install
    retval=$?

    cd ..
    rm -r $TAG.tar.gz cJSON-$VERSION
    return $retval
}

case $PLATFORM in
linux-deb)
    apt-get update
    apt-get install -y g++ make libssl-dev libyaml-dev || exit $?

    apt-get install -y libcjson-dev

    case $? in
    0)
        ;;
    100)
        install_cjson || exit $?
        ;;
    *)
        exit $?
    esac
    ;;

linux-rpm)
    yum install -y gcc-c++ make openssl-devel libyaml-devel || exit $?
    install_cjson || exit $?
    ;;

darwin)
    which brew 2> /dev/null || CI= /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" || exit $?
    brew install libyaml cjson || exit $?
    ;;

freebsd)
    pkg install -y gcc gmake libyaml libcjson || exit $?

esac
