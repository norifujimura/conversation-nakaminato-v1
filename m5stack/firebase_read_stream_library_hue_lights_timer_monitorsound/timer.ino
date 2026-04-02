void timeCheck(){
  //previousTimerState = timerState;

  //state = "off";
  /*
  if(month == 11){
    if(startHourOne <= hour && hour < endHourOne){
      timerState = "lightOn";
    }
    
    if(startHourTwo <= hour && hour < endHourTwoOne){
      timerState = "lightOn";
    }
  }else{
     if(startHourOne <= hour && hour < endHourOne){
      timerState = "lightOn";
    }
    
    if(startHourTwo <= hour && hour < endHourTwoTwo){
      timerState = "lightOn";
    }
  }
  */

  /*
  if(startHour <= hour && hour < endHour){
    timerState = "on";
  }
  */

  if(isOnAcrossMidnight){
    //this case it startHour always larger than endHour
    if(startHour < hour && hour <= 24){
      timerState = "on";
      return;
    }else if (hour < endHour){
      timerState = "on";
      return;
    }

    if(hour < startHour){
      timerState = "off";
      return;
    }

    if(endHour < hour){
      timerState = "off";
      return;
    }

    if(hour == startHour){
      if(startMin <= minute){
        timerState = "on";
      }else{
        timerState = "off";
        return;
      }
    }

    if(hour == endHour){
      if(minute < endMin){
        timerState = "on";
      }else{
        timerState = "off";
        return;
      }
    }

  }else{
    if(startHour < hour && hour < endHour){
      timerState = "on";
      return;
    }

    if(startHour < hour && endHour < hour){
      timerState = "off";
      return;
    }

    if(hour < startHour && hour < endHour){
      timerState = "off";
      return;
    }

    if(hour == startHour){
      if(startMin <= minute){
        timerState = "on";
      }else{
        timerState = "off";
        return;
      }
    }

    if(hour == endHour){
      if(minute < endMin){
        timerState = "on";
      }else{
        timerState = "off";
        return;
      }
    }
  }
  
  /*
  if(startHour <= hour){
    if(hour == startHour){
      if(startMin <= minute){
        timerState = "on";
      }
    }else{
      if(hour <= endHour){
        if(hour == endHour){
          if(minute <= endMin){
            timerState = "on";
          }
        }
      }else{
        timerState = "on";
      }
    }
  }
  */
}


