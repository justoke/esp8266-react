import React, { Component, Fragment } from 'react';

import { withStyles } from '@material-ui/core/styles';
import Button from '@material-ui/core/Button';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemText from '@material-ui/core/ListItemText';
import ListItemAvatar from '@material-ui/core/ListItemAvatar';
import Avatar from '@material-ui/core/Avatar';
import Divider from '@material-ui/core/Divider';
import SettingsInputAntennaIcon from '@material-ui/icons/SettingsInputAntenna';
import RefreshIcon from '@material-ui/icons/Refresh';

import { restComponent } from '../components/RestComponent';
import LoadingNotification from '../components/LoadingNotification';
import SectionContent from '../components/SectionContent'

import * as Highlight from '../constants/Highlight';
import { MQTT_STATUS_ENDPOINT } from '../constants/Endpoints';

const styles = theme => ({
  ["mqttStatus_" + Highlight.SUCCESS]: {
    backgroundColor: theme.palette.highlight_success
  },
  ["mqttStatus_" + Highlight.IDLE]: {
    backgroundColor: theme.palette.highlight_idle
  },
  button: {
    marginRight: theme.spacing(2),
    marginTop: theme.spacing(2),
  }
});

class MQTTStatus extends Component {

  componentDidMount() {
    this.props.loadData();
  }

  mqttStatusHighlight(data) {
    return data.active ? Highlight.SUCCESS : Highlight.IDLE;
  }

  mqttStatus(data) {
    return data.active ? "Active" : "Inactive";
  }

  createListItems(data, classes) {
    return (
      <Fragment>
        <ListItem>
          <ListItemAvatar>
            <Avatar className={classes["mqttStatus_" + this.mqttStatusHighlight(data)]}>
              <SettingsInputAntennaIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="Status" secondary={this.mqttStatus(data)} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar>IP</Avatar>
          </ListItemAvatar>
          <ListItemText primary="IP Address" secondary={data.ip_address} />
        </ListItem>
        <Divider variant="inset" component="li" />
      </Fragment>
    );
  }

  renderMQTTStatus(data, classes) {
    return (
      <div>
        <List>
          {this.createListItems(data, classes)}
        </List>
        <Button startIcon={<RefreshIcon />} variant="contained" color="secondary" className={classes.button} onClick={this.props.loadData}>
          Refresh
        </Button>
      </div>
    );
  }

  render() {
    const { fetched, errorMessage, data, loadData, classes } = this.props;
    return (
      <SectionContent title="MQTT Status">
        <LoadingNotification
          onReset={loadData}
          fetched={fetched}
          errorMessage={errorMessage}
          render={
            () => this.renderMQTTStatus(data, classes)
          }
        />
      </SectionContent>
    )
  }
}

export default restComponent(MQTT_STATUS_ENDPOINT, withStyles(styles)(MQTTStatus));
