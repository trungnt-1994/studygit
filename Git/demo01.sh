#!/bin/bash
if [ "$#" -eq 0 ]
then echo "please enter package's name and attribute name"
exit 0
else
case $2
in
	"")
		apt-cache show $1
		;;
	name) 
		echo "show package's name"
		apt-cache show $1 | grep ^Package:
		;;
	version) 
		echo "show package's version"
		apt-cache show $1 | grep ^Version: 
		;;
	installed) 
		echo "show package's installed size"
		apt-cache show $1 | grep ^Installed-Size:
		;;
	maintainer)
		echo "show package's maintainer"
		apt-cache show $1 | grep ^Maintainer:
		;;
	architecture)
		echo "show package's architecture"
		apt-cache show bc | grep ^Architecture:
		;;
	homepage)
                echo "show package's homepage"
                apt-cache show bc | grep ^Homepage:
                ;;
	tag)
		echo "show package's tags"
		apt-cache show bc | grep ^Tag:
                ;;	
        section)
                echo "show package's section"    
                apt-cache show bc | grep ^Section:
                ;;
        priority)
                echo "show package's priority"    
                apt-cache show bc | grep ^Priority:
                ;;
        filename)
                echo "show package's filename"    
                apt-cache show bc | grep ^Filename:
                ;;
        size)
                echo "show package's sizes"    
                apt-cache show bc | grep ^Size:
                ;;

	description) 
		echo "show package's description"
		apt-cache show $1 | grep ^Description
		;;	
	depend)
		echo "show package's depends"
		apt-cache depends $1
		;;
	rdepend)
		echo "show package's reverse depends"
		apt-cache rdepends $1
		;;
	provide) 
		echo "show package's provide"
		apt-cache show $1 | grep ^Provide:
		;; 
	files) 
		echo "list all files located inside $1"
		apt-file list $1 |more
		;;
	*) echo "choose menu: 
		name) show pkgs name
		version) show pkgs version
		installed) show pkgs installed-size
		maintainer) show pkgs maintainer
		architecture) show pkgs description architecture
		depend) show pkgs depends
		rdepend) show pkgs reverse depends
		description) show pkgs description
                homepage) show pkgs homepage
                tag) show pkgs tags
                section) show pkgs section
                priority) show pkgs description priority
                filename) show pkgs filename
                size) show pkgs sizes
                files) show all file located inside a pkg"
			
esac
fi
