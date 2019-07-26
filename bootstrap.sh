#! /bin/bash
# July 13, 2019

platform() {
    if [ -f /etc/os-release ]
    then
        . /etc/os-release

        case $ID in
        debian|ubuntu|raspbian)
            echo "linux-deb"
            return 0
            ;;
        fedora|centos)
            echo "linux-rpm"
            return 0
            ;;
        *)
            return 1
        esac
    elif [ -f /etc/redhat-release ]
    then
        echo "linux-deb"
        return 0
    else
        return 1
    fi
}

PLATFORM=$(platform)

if [ $? != 0 ]
then
    echo ERROR: Unsupported platform. >&2
    exit 1
fi

install_cjson() {
    TAG=$(curl -s https://api.github.com/repos/DaveGamble/cJSON/releases/latest | grep -oP '"tag_name": "\K(.*)(?=")') || exit $?
    VERSION=${TAG#v}

    curl -OL https://github.com/DaveGamble/cJSON/archive/$TAG.tar.gz || exit $?
    tar -zxf $TAG.tar.gz
    cd cJSON-$VERSION
    make && make install
    R=$?

    cd ..
    rm -r $TAG.tar.gz cJSON-$VERSION
    return $R
}

case $PLATFORM in
linux-deb)
    apt-get update
    apt-get install -y g++ make libssl-dev libyaml-dev curl || exit $?

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
    yum install -y g++ make openssl-devel libyaml-devel curl || exit $?
    install_cjson || exit $?
    ;;

esac
