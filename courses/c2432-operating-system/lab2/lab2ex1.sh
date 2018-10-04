#Which subject sucks?
read -p "Which subject have you performed worst? " code
case $code in
    comp1*)     echo "$code is just a Level 1 subject" ;;
    comp2*)     echo "$code is an intermediate Level 2 subject" ;;
    comp[34]*)  echo "$code is a challenging subject" ;;
    comp[56]*)  echo "F**k this shit" ;;
    comp*)      echo "??????" ;;
    *)          echo "......" ;;
esac
