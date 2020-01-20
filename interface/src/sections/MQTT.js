import React, { Component } from 'react';
import { Redirect, Switch } from 'react-router-dom'

import Tabs from '@material-ui/core/Tabs';
import Tab from '@material-ui/core/Tab';

import AuthenticatedRoute from '../authentication/AuthenticatedRoute';
import MenuAppBar from '../components/MenuAppBar';
import MQTTSettings from '../containers/MQTTSettings';
import MQTTStatus from '../containers/MQTTStatus';
import { withAuthenticationContext } from '../authentication/Context.js';

class MQTT extends Component {

  handleTabChange = (event, path) => {
    this.props.history.push(path);
  };

  render() {
    const { authenticationContext } = this.props;
    return (
      <MenuAppBar sectionTitle="MQTT">
        <Tabs value={this.props.match.url} onChange={this.handleTabChange} indicatorColor="primary" textColor="primary" variant="fullWidth">
          <Tab value="/mqtt/status" label="MQTT Status" />
          <Tab value="/mqtt/settings" label="MQTT Settings" disabled={!authenticationContext.isAdmin()} />
        </Tabs>
        <Switch>
          <AuthenticatedRoute exact={true} path="/mqtt/status" component={MQTTStatus} />
          <AuthenticatedRoute exact={true} path="/mqtt/settings" component={MQTTSettings} />
          <Redirect to="/mqtt/status" />
        </Switch>
      </MenuAppBar>
    )
  }
}

export default withAuthenticationContext(MQTT);
