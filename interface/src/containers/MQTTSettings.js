import React, { Component } from 'react';

import { MQTT_SETTINGS_ENDPOINT } from '../constants/Endpoints';
import { restComponent } from '../components/RestComponent';
import LoadingNotification from '../components/LoadingNotification';
import SectionContent from '../components/SectionContent';
import MQTTSettingsForm from '../forms/MQTTSettingsForm';

class MQTTSettings extends Component {

  componentDidMount() {
    this.props.loadData();
  }

  render() {
    const { fetched, errorMessage, data, saveData, loadData, handleValueChange, handleCheckboxChange } = this.props;
    return (
      <SectionContent title="MQTT Settings">
        <LoadingNotification
          onReset={loadData}
          fetched={fetched}
          errorMessage={errorMessage}
          render={() =>
            <MQTTSettingsForm
              mqttSettings={data}
              onSubmit={saveData}
              onReset={loadData}
              handleValueChange={handleValueChange}
              handleCheckboxChange={handleCheckboxChange}
            />
          }
        />
      </SectionContent>
    )
  }

}

export default restComponent(MQTT_SETTINGS_ENDPOINT, MQTTSettings);
